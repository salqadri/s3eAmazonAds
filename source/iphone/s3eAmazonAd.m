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

#include "s3eAmazonAd.h"
#include "s3eAmazonAds_impl.h"
#import <AmazonAd/AmazonAdOptions.h>

@implementation S3EAmazonAd

- (id) init:(S3eAmazonAdSngl*) parent
{
    if (self = [super init])
    {
        self->m_Parent = parent;
    }
    return self;
}

- (void) dealloc
{
    if (m_AdView)
    {
        [m_AdView removeFromSuperview];
        [m_AdView release];
    }
    [m_AdInterstitial release];
    [super dealloc];
}

-(BOOL) prepareAdLayout:(s3eAmazonAdsPosition)position size:(s3eAmazonAdsSize)size
                  width:(int)width height:(int)height
{
    // in theory we pass these to the adView. However it does not really work that way:
    // the size is used on init and other stuff is used elsewhere.
    m_Position = position;
    m_Size = size;
    m_Width = width;
    m_Height = height;
    m_LayoutSet = YES; // to say the other parameters have been set
    return !m_AdView; // if we have an adView then probably too late
}

-(BOOL) setAdTargetingOptions:(s3eAmazonAdsTargetingOptions*)options
{
    m_Options = options;
    return !m_AdView; // if we have an adView then probably too late
}

-(s3eAmazonAdsTargetingOptions*) getAdTargetingOptions
{
    return m_Options;
}

-(AmazonAdOptions*) setUpOptions
{
    AmazonAdOptions* options = [AmazonAdOptions options];
    if (m_Options)
    {
        options.usesGeoLocation = m_Options->m_EnableGeoLocation;
        // TODO not sure what to do with mask and floor price

        // Always add advanced option: slot="marmalade sdk" 
        [options setAdvancedOption:@"marmalade sdk" forKey:@"slot"];
    }
    // TODO handle advanced options
    options.isTestRequest = [m_Parent areTesting];
    return options;
}

const static struct
{
    s3eAmazonAdsSize m_Enum;
    int m_Width;
    int m_Height;
} kSizeLookup[] =
{
    {S3E_AMAZONADS_SIZE_300x50, 300, 50},
    {S3E_AMAZONADS_SIZE_320x50, 320, 50},
    {S3E_AMAZONADS_SIZE_300x250, 300, 250},
    // {S3E_AMAZONADS_SIZE_600x90, 600, 90}, Not currently supported on iOS
    {S3E_AMAZONADS_SIZE_728x90, 728, 90},
    {S3E_AMAZONADS_SIZE_1024x50, 1024, 50},
    {-1, 0, 0}
};

-(CGSize) equivSize
{
    // return size to use based on original enum
    
    int width = 0;
    int height = 0;
    
    int indx=0;
    BOOL found = NO;
    while (kSizeLookup[indx].m_Enum >= 0)
    {
        if (kSizeLookup[indx].m_Enum == m_Size)
        {
            width = kSizeLookup[indx].m_Width;
            height = kSizeLookup[indx].m_Height;
            found = YES;
            break;
        }
        indx ++;
    }
    
    if (!found)
    {
        // special cases?
        if (m_Size == S3E_AMAZONADS_SIZE_CUSTOM)
        {
            width = m_Width;
            height = m_Height;
        }
        else if (m_Size == S3E_AMAZONADS_SIZE_AUTO)
        {
            // AUTO is covered explicitly on the android version but not currently on iOS.
            // To support, find the known size just smaller than the main view.
            int mainViewWidth = [self viewControllerForPresentingModalView].view.frame.size.width;
            indx = 0;
            while (kSizeLookup[indx].m_Enum >= 0)
            {
                // if the width in table is greater than what we have so far and less then
                // screen, then use that table entry for both width and height
                int tempWidth = kSizeLookup[indx].m_Width;
                if (tempWidth > width && tempWidth <= mainViewWidth)
                {
                    width = tempWidth;
                    height = kSizeLookup[indx].m_Height;
                }
                indx++;
            }
        }
        else
        {
            // Unknown size - default to 0. Will probably fail below.
        }
    }
    CGSize result;
    result.width = width;
    result.height = height;
    return result;
}

-(BOOL) loadAd:(BOOL)show timeout:(int)timeout
{
    // if we've already got an AMA object, throw it away
    [m_AdView removeFromSuperview];
    [m_AdView release];
    m_AdView = nil;

    [m_AdInterstitial release];
    m_AdInterstitial = nil;
    
    CGSize sizeToRequest = [self equivSize];
    m_AdView = [AmazonAdView amazonAdViewWithAdSize:sizeToRequest];
    [m_AdView retain];
    m_AdView.delegate = self;
    AmazonAdOptions* options = [self setUpOptions];
    if (timeout > 0)
        options.timeout = timeout;
    m_Loading = YES;
    m_ShowOnLoad = show;
    [m_AdView loadAd:options];
    
    if (show)
    {
        return [self showAd]; // use public showAd API
    }
    return YES;
}

-(BOOL) loadInterstitialAd
{
    // if we've already got an AMA object, throw it away - if needs will effectively cancel
    if (m_AdView || m_AdInterstitial)
    {
        [m_AdView release];
        m_AdView = nil;
    }
    if (m_AdInterstitial)
    {
        [m_AdInterstitial release];
        m_AdInterstitial = nil;
    }
    
    m_AdInterstitial = [AmazonAdInterstitial amazonAdInterstitial];
    [m_AdInterstitial retain];
    m_AdInterstitial.delegate = self;
    AmazonAdOptions* options = [self setUpOptions];
    m_Loading = YES;
    m_ShowOnLoad = NO;
    [m_AdInterstitial load:options];
    return YES;
}

-(BOOL) showAd
{
    if (!(m_AdView || m_AdInterstitial))
    {
        return NO;
    }
    
    if (m_Loading)
    {
        return NO;
    }

    UIViewController* viewController = [self viewControllerForPresentingModalView];
    if (m_AdView)
    {
        CGSize screenSize = viewController.view.frame.size;
        CGSize adSize = m_AdView.frame.size;

        if ((NSFoundationVersionNumber <= NSFoundationVersionNumber_iOS_7_1) && UIInterfaceOrientationIsLandscape([UIApplication sharedApplication].statusBarOrientation))
        {
            // iOS versions prior to 7.1 use orientation independent resolution
            // so we need to swap width/height in lanscape
            screenSize = CGSizeMake(viewController.view.frame.size.height, viewController.view.frame.size.width);
        }

        // banner ad placement
        CGPoint adCenter;
        adCenter.x = screenSize.width / 2;
        adCenter.y = adSize.height / 2;

        if (m_Position == S3E_AMAZONADS_POSITION_BOTTOM)
        {
            adCenter.y = screenSize.height - adCenter.y;
        }

        m_AdView.center = adCenter;
        [viewController.view addSubview:m_AdView];
    }
    else
    {
        // interstitial
        [m_AdInterstitial presentFromViewController:viewController];
    }
    return YES;
}

-(BOOL) inspectAd:(s3eAmazonAdsAdInfo*) info
{
    info->m_IsLoading = m_Loading;
    return YES;
}


// AmazonAdViewDelegate

- (UIViewController*)viewControllerForPresentingModalView
{
    return [m_Parent viewControllerToUse];
}

- (void)adViewWillExpand:(AmazonAdView *)view
{
    if (!m_AdView || view != m_AdView)
        return; // ignore if not expected view - just in case
    [self onAction:AmazonAdActionExpanded];
}

- (void)adViewDidCollapse:(AmazonAdView *)view
{
    if (!m_AdView || view != m_AdView)
        return; // ignore if not expected view - just in case
    [self onAction:AmazonAdActionCollapsed];
}

- (void)adViewDidFailToLoad:(AmazonAdView *)view withError:(AmazonAdError *)error
{
    if (!m_AdView || view != m_AdView)
        return; // ignore if not expected view - just in case
    m_Loading = NO;
    [self onError:error];
}
 
- (void)adViewDidLoad:(AmazonAdView *)view
{
    if (!m_AdView || view != m_AdView)
        return; // ignore if not expected view - just in case
    m_Loading = NO;
    [self onLoaded];
}

// AmazonAdInterstitialDelegate

- (void)interstitialDidLoad:(AmazonAdInterstitial *)interstitial
{
    if(!m_AdInterstitial || interstitial != m_AdInterstitial)
        return; // ignore if not expected - just in case
    m_Loading = NO;
    [self onLoaded];
}

- (void)interstitialDidFailToLoad:(AmazonAdInterstitial *)interstitial withError:(AmazonAdError *)error
{
    if(!m_AdInterstitial || interstitial != m_AdInterstitial)
        return; // ignore if not expected - just in case
    m_Loading = NO;
    [self onError:error];
}

- (void)interstitialDidDismiss:(AmazonAdInterstitial *)interstitial
{
    if(!m_AdInterstitial || interstitial != m_AdInterstitial)
        return; // ignore if not expected - just in case
    [self onAction:AmazonAdActionDismissed];
}

- (void) onLoaded
{
    // if show was given to orig load, then we show at this point
    if (m_ShowOnLoad)
    {
        m_ShowOnLoad = NO; // should not make a difference, but clear anyway
        [self showAd];
    }
    // handle ...DidLoad calls. Setup LoadedData and callback
    m_LoadedProperties.m_AdType = m_AdInterstitial ? S3E_AMAZONADS_TYPE_INTERSTITIAL :
                  S3E_AMAZONADS_TYPE_UNKNOWN;
    [m_Parent onLoaded:self properties:&m_LoadedProperties];
}

- (void) onError:(AmazonAdError*)error
{
    [m_Parent onError:self error:error];
}

- (void) onAction:(AmazonAdAction)action
{
    [m_Parent onAction:self action:action];
}

@end
