/*
 * (C) 2001-2012 Marmalade. All Rights Reserved.
 *
 * This document is protected by copyright, and contains information
 * proprietary to Marmalade.
 *
 * This file consists of source code released by Marmalade under
 * the terms of the accompanying End User License Agreement (EULA).
 * Please do not use this program/source code before you have read the
 * EULA and have agreed to be bound by its terms.
 */

/*
java implementation of the s3eAmazonAds extension.
*/

package com.ideaworks3d.marmalade.s3eAmazonAds;

import com.ideaworks3d.marmalade.LoaderActivity;
import com.ideaworks3d.marmalade.LoaderAPI;
import com.ideaworks3d.marmalade.ResourceUtility;

import android.app.Activity;
import android.view.*;
import android.widget.LinearLayout;
import android.widget.FrameLayout;
import android.widget.FrameLayout.LayoutParams;
import android.util.Log;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.Map;
import java.util.Map.Entry;

import android.util.DisplayMetrics;
import android.view.Display;

import com.amazon.device.ads.*;
import com.amazon.device.ads.AdProperties.AdType;
import com.ideaworks3d.marmalade.s3eAmazonAds.s3eAmazonAdsListener;

public class s3eAmazonAds
{
    // Globals
    private LoaderActivity g_Activity = LoaderAPI.getActivity();
    private float g_Density;
    private int g_Width;
    private int g_Height;

    // private float g_Scale;
    private static final int InvalidID = 0;
    private static final String LOG_TAG = "s3eAmazonAds";

    // Be sure to update these to match the enums defined in the s4e
    public final static int S3E_AMAZONADS_ERR_NONE                     = 0;
    public final static int S3E_AMAZONADS_ERR_UNEXPECTED               = 1;
    public final static int S3E_AMAZONADS_ERR_INVALID_APPKEY           = 2;
    public final static int S3E_AMAZONADS_ERR_INVALID_SIZE             = 3;
    public final static int S3E_AMAZONADS_ERR_INVALID_OPTION           = 4;
    public final static int S3E_AMAZONADS_ERR_NULL_PARAM               = 5;
    public final static int S3E_AMAZONADS_ERR_INVALID_ID               = 6;
    public final static int S3E_AMAZONADS_ERR_INVALID_POSITION         = 7;
    public final static int S3E_AMAZONADS_ERR_ALREADY_COLLAPSED        = 8;
    public final static int S3E_AMAZONADS_ERR_CANNOT_COLLAPSE          = 9;
    public final static int S3E_AMAZONADS_ERR_REGISTRATION_EXCEPTION   = 10;
    public final static int S3E_AMAZONADS_ERR_BUSY_LOADING             = 11;
    public final static int S3E_AMAZONADS_ERR_ID_USED_FOR_INTERSTITIAL = 12;
    public final static int S3E_AMAZONADS_ERR_NULL_AD                  = 13;
    public final static int S3E_AMAZONADS_ERR_SHOW_FAILED              = 14;
    public final static int S3E_AMAZONADS_ERR_ALREADY_SHOWING          = 15;
    public final static int S3E_AMAZONADS_ERR_NOT_LOADED               = 16;

    public final static int S3E_AMAZONADS_SIZE_CUSTOM   = -1;
    public final static int S3E_AMAZONADS_SIZE_AUTO     = 0;
    public final static int S3E_AMAZONADS_SIZE_300x50   = 1;
    public final static int S3E_AMAZONADS_SIZE_320x50   = 2;
    public final static int S3E_AMAZONADS_SIZE_300x250  = 3;
    public final static int S3E_AMAZONADS_SIZE_600x90   = 4;
    public final static int S3E_AMAZONADS_SIZE_728x90   = 5;
    public final static int S3E_AMAZONADS_SIZE_1024x50  = 6;
    public final static int S3E_AMAZONADS_SIZE_MAX      = 6;

    public final static int S3E_AMAZONADS_POSITION_TOP    = 1;
    public final static int S3E_AMAZONADS_POSITION_BOTTOM = 2;

    // ENUM MAPPINGS s3e to Amazon Mobile Ads
    public static AdSize s3eAdSizeMap[] = {
        AdSize.SIZE_AUTO, AdSize.SIZE_300x50, AdSize.SIZE_320x50, AdSize.SIZE_300x250, AdSize.SIZE_600x90, AdSize.SIZE_728x90, AdSize.SIZE_1024x50};

    //
    // s3eAmazonAdsManager
    // class that manages per Ad data for managing ads.
    //
    public static class s3eAmazonAdsManager
    {
        private static HashMap<Integer, s3eAdData> m_LUT = new HashMap<Integer, s3eAdData>();
        private static int g_Id = InvalidID;


        public static int RegisterAdData()
        {
            // increment g_ID - use that new value
            g_Id++; // will start at 1 (InvalidID + 1)

            // Instantiate new s3eAdData object
            s3eAdData data = new s3eAdData(g_Id);

            if (data != null && data.m_Id > InvalidID)
            {
                // Put the object in the hashmap keyed by g_Id
                m_LUT.put(g_Id, data);

                // Set advanced option: slot="marmaladesdk"
                s3eAdOptions options = data.GetOptions();
                options.setAdvancedPair("slot", "marmaladesdk");

                // return the id of the stored object
                return data.m_Id;
            }

            return InvalidID;
        }

        public static boolean IsValidId(int id)
        {
            // valid ids start at IsValidId + 1: 1, 2, 3...
            if (id <= InvalidID)
                return false;

            // all other ids are only valid if it is a key
            // in the m_LUT hashmap
            Integer idx = id;
            if (m_LUT.containsKey(idx))
                return true;

            return false;
        }

        public static s3eAdData GetData(int id)
        {
            Integer idx = id;
            return m_LUT.get(idx);
        }

        public static void setLoaded(int id, boolean loaded)
        {
            Integer idx = id;
            if (m_LUT.containsKey(idx))
            {
                s3eAdData data = m_LUT.get(idx);
                data.m_Loaded = loaded;
            }
        }

        // release all resources associated with id
        // remove id from table
        public static void Destroy(int id)
        {
            Integer idx = id;
            if (m_LUT.containsKey(idx))
            {
                s3eAdData data = m_LUT.get(idx);
                data.Destroy();
                m_LUT.remove(idx);
            }
        }

        public static void DestroyAll()
        {
            for (Entry<Integer, s3eAdData> entry : m_LUT.entrySet())
            {
                entry.getValue().Destroy();
            }
            m_LUT.clear();
        }
    }

    //
    // Internal class s3eAdData storing per Ad data
    // Instances of this class are added to a hashmap keyed by a unique identifier.
    //
    public static class s3eAdData
    {
        public int m_Id;                // per Ad id: corresponds to s3eAmazonAdsId based back to user
        private boolean m_Loaded;       // set on load
        private boolean m_Visible;      // set when shown


        public s3eAdOptions m_Options;

        private AdLayout m_AdView;
        private InterstitialAd m_InterstitialAd;
        private AdSize m_AdSize;
        private int m_Position;

        private void init(int id)
        {
            m_Id = id;

            // state
            m_Visible = false;
            m_Loaded = false;

            // options
            m_Options = new s3eAdOptions();
            if (m_Options == null || !m_Options.IsValid())
                m_Id = InvalidID;

            // layout
            m_AdView = null;
            m_AdSize = AdSize.SIZE_AUTO;
            m_Position = S3E_AMAZONADS_POSITION_TOP;

            // interstitial
            m_InterstitialAd = null;
        }

        public void Destroy()
        {
            if (m_AdView != null)
            {
                AdLayout view = m_AdView;
                m_AdView = null;
                view.destroy();
            }

            if (m_InterstitialAd != null)
            {
                m_InterstitialAd = null;
            }
        }

        public s3eAdData()
        {
            init(InvalidID);
        }

        public s3eAdData(int id)
        {
            init(id);
        }

        public int getId()
        {
            return m_Id;
        }

        public s3eAdOptions GetOptions()
        {
            return m_Options;
        }

        public boolean UsesOptions()
        {
            return (m_Options._advanced || m_Options._age || m_Options._geo || m_Options._price);
        }

    }

    //
    // s3eAdOptions
    //
    //
    public static class s3eAdOptions
    {
        s3eAdOptions()
        {
            // m_Keys is required to keep track of which keys
            // are stored in AdTargetingOptions as the underlying hashmap isn't public.
            m_Keys = new LinkedHashSet<String>();
            m_AdOptions = new AdTargetingOptions();
            _advanced = _price = _age = _geo = false;
        }

        public boolean IsValid()
        {
            return ((m_Keys != null) && (m_AdOptions != null));
        }

        //
        // Called from c++
        //
        public int setOptions(int price, boolean update_price, int age, boolean update_age, boolean geolocation, boolean update_geo, String advanced, boolean update_advanced)
        {
            Log.i(LOG_TAG, "setOptions ----");

            if (update_price)
                setPrice(price);
            if (update_age)
                setAge(age);
            if (update_geo)
                setGeo(geolocation);
            if (update_advanced)
                setAdvanced(advanced);
            return S3E_AMAZONADS_ERR_NONE;
        }

        // Note we pass options back to c++ using int array storing:
        //   errcode, price, _price, age, _age, geolocation, _geo
        // with return value being the key/value/action triplet strings separated by newline
        public String getOptions(int[] opt)
        {
            Log.i(LOG_TAG, "getOptions ----");

            // we assume optarr is big enough (allocated on c++ side)
            opt[0] = S3E_AMAZONADS_ERR_NONE;
            opt[1] = (int) m_AdOptions.getFloorPrice();
            opt[2] = _price ? 1 : 0;
            opt[3] = m_AdOptions.getAge();
            opt[4] = _age ? 1 : 0;
            opt[5] = m_AdOptions.isGeoLocationEnabled() ? 1 : 0;
            opt[6] = _geo ? 1 : 0;

            if (_advanced == false)
                return null;

            StringBuffer buffer = new StringBuffer();
            int i = 0;
            Iterator<String> it = m_Keys.iterator();
            while (it.hasNext())
            {
                String key = (String) it.next();
                buffer.append(key);
                buffer.append('\n');
                buffer.append(m_AdOptions.getAdvancedOption(key));
                buffer.append('\n');
                buffer.append("add");
                buffer.append('\n');
                i++;
            }
            return buffer.toString();
        }

        //
        // Setters:
        //
        public void setPrice(int price)
        {
            Log.i(LOG_TAG, "setPrice: " + price);
            m_AdOptions.setFloorPrice(price);
            _price = true;
        }

        public void setAge(int age)
        {
            Log.i(LOG_TAG, "setAge: " + age);
            m_AdOptions.setAge(age);
            _age = true;
        }

        public void setGeo(boolean geo)
        {
            Log.i(LOG_TAG, "setGeo: " + geo);
            m_AdOptions.enableGeoLocation(geo);
            _geo = true;
        }

        public void setAdvanced(String keyvalues)
        {
            String lines[] = keyvalues.split("\\n");
            int imax = lines.length / 3;
            imax *= 3;

            for (int i=0; i<imax; i+=3)
            {
                if (lines[i+2].equalsIgnoreCase("del"))
                    setAdvancedPair(lines[i], null);
                else
                    setAdvancedPair(lines[i], lines[i+1]);
            }
        }

        public void setAdvancedPair(String key, String value)
        {
            if (value == null)
                Log.i(LOG_TAG, "setAdvanced: removing " + key);
            else
                Log.i(LOG_TAG, "setAdvanced: adding " + key + " " + value);

            m_AdOptions.setAdvancedOption(key, value);
            _advanced = true;

            if (value == null)
            {
                m_Keys.remove(key);
                if (m_Keys.size() == 0)
                    _advanced = false;
            }
            else
            {
                m_Keys.add(key);
            }
        }

        // AdTargetingOptions
        private AdTargetingOptions m_AdOptions;
        LinkedHashSet<String> m_Keys;

        // fields enabled
        // only enabled fields are copied to AdTargetingOptions object
        private boolean _price;
        private boolean _age;
        private boolean _geo;
        private boolean _advanced;
    }


    //
    // s3eAmazonAds API java layer
    //
    public int s3eAmazonAdsInit(String appKey, boolean enableTesting, boolean enableLogging)
    {
        DisplayMetrics metrics = new DisplayMetrics();

        LoaderAPI.getActivity().getWindowManager().getDefaultDisplay().getMetrics(metrics);

        g_Density=metrics.density;
        g_Width=metrics.widthPixels;
        g_Height=metrics.heightPixels;
        Log.i(LOG_TAG, "s3eAmazonAdsInit width:" + g_Width + " height:" + g_Height + " density:" + g_Density);

        try
        {
            AdRegistration.enableLogging(enableLogging);
            AdRegistration.enableTesting(enableTesting);
            AdRegistration.setAppKey(appKey);
        }
        catch (final Exception e)
        {
            Log.e(LOG_TAG, "Exception thrown: " + e.toString());
            return S3E_AMAZONADS_ERR_REGISTRATION_EXCEPTION;
        }

        return S3E_AMAZONADS_ERR_NONE;
    }

    public int s3eAmazonAdsTerminate()
    {
        Log.i(LOG_TAG, "s3eAmazonAdsTerminate");

        s3eAmazonAdsManager.DestroyAll();

        return S3E_AMAZONADS_ERR_NONE;
    }

    //
    // s3eAmazonAdsPrepareAd prepares for loading an Ad by allocating ad data in a LUT.
    // It returns the identifier into this LUT.
    //
    public int s3eAmazonAdsPrepareAd(int[] pId)
    {
        // Create instance of s3eAdData
        // Store in global look up table
        // All ads before and after load are tracked by this LUT.
        int id = s3eAmazonAdsManager.RegisterAdData();

        // Log the result
        Log.i(LOG_TAG, "s3eAmazonAdsPrepareAd generated Id: " + id);

        // Store in pId[0] to return to user
        pId[0] = id;

        // id < 0 is invalid and should not happen.
        if (id <= InvalidID)
            return S3E_AMAZONADS_ERR_UNEXPECTED;

        return S3E_AMAZONADS_ERR_NONE;
    }

    public int s3eAmazonAdsPrepareAdLayout(int id, int position, int size, int width, int height)
    {
        Log.i(LOG_TAG, "s3eAmazonAdsPrepareAdLayout id: " + id);

        if (!s3eAmazonAdsManager.IsValidId(id))
            return S3E_AMAZONADS_ERR_INVALID_ID;

        // validate arguments
        if (position != S3E_AMAZONADS_POSITION_TOP && position != S3E_AMAZONADS_POSITION_BOTTOM)
            return S3E_AMAZONADS_ERR_INVALID_POSITION; // invalid position

        if ((size < S3E_AMAZONADS_SIZE_CUSTOM) || (size > S3E_AMAZONADS_SIZE_MAX) || (size == S3E_AMAZONADS_SIZE_CUSTOM && (width<=0 || height<=0)))
            return S3E_AMAZONADS_ERR_INVALID_SIZE; // invalid size

        // get the ad data object
        s3eAdData data = s3eAmazonAdsManager.GetData(id);

        // update the size
        if (size == S3E_AMAZONADS_SIZE_CUSTOM)
            data.m_AdSize = new AdSize(width, height);
        else
            data.m_AdSize = s3eAdSizeMap[size];

        // update the position
        data.m_Position = position;

        return S3E_AMAZONADS_ERR_NONE; // success
    }

    public s3eAdOptions s3eAmazonAdsGetAdOptions(int id)
    {
        Log.i(LOG_TAG, "s3eAmazonAdsGetAdOptions id: " + id);

        if (!s3eAmazonAdsManager.IsValidId(id))
            return null;

        s3eAdData data = s3eAmazonAdsManager.GetData(id);
        s3eAdOptions options = data.GetOptions();

        return options;
    }

    private ViewGroup.LayoutParams getLayoutParams(int position, boolean autosize, int w, int h)
    {
        ViewGroup.LayoutParams lp;
        Log.i(LOG_TAG, "s3eAmazonAdsGetAdOptions getLayoutParams width: " + w + " height: " + h);

        if (w * g_Density > g_Width)
        {
            autosize = true;
        }
        
        lp = new FrameLayout.LayoutParams(
                autosize ? LayoutParams.MATCH_PARENT : (int)(((float)w * g_Density)),
                autosize ? LayoutParams.WRAP_CONTENT : (int)(((float)h * g_Density)),
                position==S3E_AMAZONADS_POSITION_BOTTOM ? Gravity.BOTTOM | Gravity.CENTER_HORIZONTAL : Gravity.TOP | Gravity.CENTER_HORIZONTAL);

        return lp;
    }

    public int s3eAmazonAdsLoadAd(int id, boolean show, int timeout)
    {
        if (!s3eAmazonAdsManager.IsValidId(id))
            return S3E_AMAZONADS_ERR_INVALID_ID;

        s3eAdData data = s3eAmazonAdsManager.GetData(id);

        // Check data
        if (data.m_InterstitialAd != null)
        {
            Log.i(LOG_TAG, "s3eAmazonAdsLoadAd: " + id + " use s3eAmazonAdsLoadInterstitialAd for interstitial ads.");
            return S3E_AMAZONADS_ERR_ID_USED_FOR_INTERSTITIAL;

        }
        else if (data.m_AdView != null && data.m_AdView.isLoading())
        {
            Log.i(LOG_TAG, "s3eAmazonAdsLoadAd: " + id + " BUSY LOADING.");
            return S3E_AMAZONADS_ERR_BUSY_LOADING;
        }

        Log.i(LOG_TAG, "s3eAmazonAdsLoadAd: " + id);

        // Create AdLayout if one doesn't already exist
        AdLayout adview = null;
        boolean setlistener = false;
        if (data.m_AdView == null)
        {
            // no adview exists so create one
            if (data.m_AdSize == null)
                data.m_AdView = new AdLayout(g_Activity); // auto TODO use stored size
            else
                data.m_AdView = new AdLayout(g_Activity, data.m_AdSize);
            setlistener = true;
        }
        else
        {
            // AdLayout exists so keep reference to old one
            adview = data.m_AdView;
            // Then create new one.
            Log.i(LOG_TAG, "s3eAmazonAdsLoadAd id: " + id + " size has changed for existing layout.");
            data.m_AdView = new AdLayout(g_Activity, data.m_AdSize);
            setlistener = true;
        }

        // If the adview has already been added to the view hierarchy
        // we need to remove it first.
        FrameLayout fl = LoaderAPI.getFrameLayout();
        int index;
        if (adview != null)
        {
            index = fl.indexOfChild(adview);
            if (index>=0)
                fl.removeView(adview);
        }

        if (setlistener)
        {
            s3eAmazonAdsListener listener = new s3eAmazonAdsListener(id);
            data.m_AdView.setListener(listener);
        }

        adview = data.m_AdView;

        // Create LayoutParams to layout at top or bottom
        int w=data.m_AdSize.getWidth();
        int h=data.m_AdSize.getHeight();
        ViewGroup.LayoutParams lp = getLayoutParams(data.m_Position, data.m_AdSize.equals(AdSize.SIZE_AUTO), w, h);

        // when show==false we put the view below the main view.
        // It's optional though.  We recommend users load and show banners immediately.
        if (show)
        {
            Log.i(LOG_TAG, "s3eAmazonAdsLoadAd loading and showing ad.");

            // To show we just add the adview to the main frame layout
            fl.addView(adview, lp);
            // and mark it as being visible
            data.m_Visible = true;
        }
        else
        {
            Log.i(LOG_TAG, "s3eAmazonAdsLoadAd loading and hiding ad.");

            // To just load we add the view underneath the main view
            index = fl.indexOfChild(LoaderAPI.getMainView());
            fl.addView(adview, index, lp);
            // and mark it as not being visible
            data.m_Visible = false;
        }

        boolean res;
        if (data.UsesOptions())
        {
            Log.i(LOG_TAG, "s3eAmazonAdsLoadAd loading banner with targeting options.");
            res = adview.loadAd(data.m_Options.m_AdOptions);
        }
        else
        {
            Log.i(LOG_TAG, "s3eAmazonAdsLoadAd loading banner.");
            res = adview.loadAd();
        }

        return (res == false) ? S3E_AMAZONADS_ERR_BUSY_LOADING : S3E_AMAZONADS_ERR_NONE;
    }

    public int s3eAmazonAdsShowAd(int id)
    {
        Log.i(LOG_TAG, "s3eAmazonAdsShowAd: " + id);

        if (!s3eAmazonAdsManager.IsValidId(id))
            return S3E_AMAZONADS_ERR_INVALID_ID;

        s3eAdData data = s3eAmazonAdsManager.GetData(id);

        // INTERSTITIAL SHOW
        if (data.m_InterstitialAd != null)
        {
            if (InterstitialAd.isAdShowing())
            {
                Log.i(LOG_TAG, "s3eAmazonAdsShowAd WARNING already showing interstitial");
                return S3E_AMAZONADS_ERR_SHOW_FAILED;
            }
            Log.i(LOG_TAG, "s3eAmazonAdsShowAd showing interstitial");
            return data.m_InterstitialAd.showAd() ? S3E_AMAZONADS_ERR_NONE : S3E_AMAZONADS_ERR_SHOW_FAILED;
        }

        // BANNER SHOW
        // We actually just unhide the ad.
        if (data.m_AdView != null)
        {
            if (data.m_Loaded == false)
            {
                Log.i(LOG_TAG, "s3eAmazonAdsShowAd WARNING ad has not loaded.");
                return S3E_AMAZONADS_ERR_NOT_LOADED;
            }

            if (data.m_Visible)
            {
                Log.i(LOG_TAG, "s3eAmazonAdsShowAd WARNING the banner ad is already showing.");
                return S3E_AMAZONADS_ERR_ALREADY_SHOWING;
            }

            Log.i(LOG_TAG, "s3eAmazonAdsShowAd unhiding banner");
            FrameLayout fl = LoaderAPI.getFrameLayout();
            fl.removeView(data.m_AdView);
            int w=data.m_AdSize.getWidth();
            int h=data.m_AdSize.getHeight();
            ViewGroup.LayoutParams lp = getLayoutParams(data.m_Position, data.m_AdSize.equals(AdSize.SIZE_AUTO), w, h);
            fl.addView(data.m_AdView, lp);
            data.m_Visible = true;
            return S3E_AMAZONADS_ERR_NONE;
        }

        // adview or interstital == null
        return S3E_AMAZONADS_ERR_NULL_AD;
    }

    public int s3eAmazonAdsLoadInterstitialAd(int id)
    {
        Log.i(LOG_TAG, "s3eAmazonAdsLoadInterstitialAd: " + id);

        if (!s3eAmazonAdsManager.IsValidId(id))
            return S3E_AMAZONADS_ERR_INVALID_ID;

        s3eAdData data = s3eAmazonAdsManager.GetData(id);

        // Set up new InterstitialAd
        if (data.m_InterstitialAd == null)
        {
            data.m_InterstitialAd = new InterstitialAd(g_Activity);
            if (data.m_InterstitialAd == null)
            {
                Log.i(LOG_TAG, "Failed to instantiate new InterstitialAd.");
                return S3E_AMAZONADS_ERR_UNEXPECTED;
            }

            Log.i(LOG_TAG, "Instantiated new InterstitialAd.");
            s3eAmazonAdsListener listener = new s3eAmazonAdsListener(id);
            if (listener == null)
            {
                Log.i(LOG_TAG, "s3eAmazonAdsLoadInterstitialAd failed to attach listener.");
                return S3E_AMAZONADS_ERR_UNEXPECTED; // TODO cleanup
            }

            Log.i(LOG_TAG, "Attaching new s3eAmazonAdsListener.");
            data.m_InterstitialAd.setListener(listener);
        }

        // Load the ad.
        boolean res;
        if (data.UsesOptions())
        {
            Log.i(LOG_TAG, "s3eAmazonAdsLoadInterstitialAd loading interstitial with prepared targeteing options");
            res = data.m_InterstitialAd.loadAd(data.m_Options.m_AdOptions);
        }
        else
        {
            Log.i(LOG_TAG, "s3eAmazonAdsLoadInterstitialAd loading interstitial with default targeting options");
            res = data.m_InterstitialAd.loadAd();
        }

        return (res == false) ? S3E_AMAZONADS_ERR_BUSY_LOADING : S3E_AMAZONADS_ERR_NONE;
    }

    public int s3eAmazonAdsDestroyAd(int id)
    {
        Log.i(LOG_TAG, "s3eAmazonAdsDestroyAd: " + id);

        if (!s3eAmazonAdsManager.IsValidId(id))
            return S3E_AMAZONADS_ERR_INVALID_ID;

        s3eAdData data = s3eAmazonAdsManager.GetData(id);
        if (data.m_AdView == null && data.m_InterstitialAd == null)
        {
            Log.i(LOG_TAG, "s3eAmazonAdsDestroyAd no ad found to destroy.  Ad already destroyed or not yet loaded.");
            return S3E_AMAZONADS_ERR_NULL_AD;
        }

        Log.i(LOG_TAG, "s3eAmazonAdsDestroyAd destroying banner ad.");

        s3eAmazonAdsManager.Destroy(id);
        return S3E_AMAZONADS_ERR_NONE;
    }

    public int s3eAmazonAdsInspectAd(int id, int[] info)
    {
        Log.i(LOG_TAG, "s3eAmazonAdsInspectAd: id:" + id);

        if (!s3eAmazonAdsManager.IsValidId(id))
            return S3E_AMAZONADS_ERR_INVALID_ID;

        s3eAdData data = s3eAmazonAdsManager.GetData(id);
        boolean isloading = false;
        if (data.m_AdView != null)
            isloading = data.m_AdView.isLoading();
        else if (data.m_InterstitialAd != null)
            isloading = data.m_InterstitialAd.isLoading();

        info[0] = isloading ? 1 : 0;

        return S3E_AMAZONADS_ERR_NONE;
    }
}
