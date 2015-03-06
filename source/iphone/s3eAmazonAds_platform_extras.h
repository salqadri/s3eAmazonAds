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

#ifndef S3EAMAZONADS_PLATFORM_EXTRAS_H
#define S3EAMAZONADS_PLATFORM_EXTRAS_H

#include "s3eAmazonAds_decs.h"

S3E_BEGIN_C_DECL

// calls from s3eAmazonAds singleton code to platform code

void s3eAmazonAds_onLoad(struct s3eAmazonAdsCallbackLoadedData* data);
void s3eAmazonAds_onError(id adId, int errorCode);
void s3eAmazonAds_onAction(id adId, AmazonAdAction action);

S3E_END_C_DECL

#endif
