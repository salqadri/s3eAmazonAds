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

#ifndef S3EAMAZONADS_DECS_H
#define S3EAMAZONADS_DECS_H

// Various simple declarations shared between the various source files in this extension implementation.
// Should be valid in both C++ and straight objective-C

S3E_BEGIN_C_DECL

typedef enum
{
    AmazonAdActionExpanded, // warning: should match equiv values of s3eAmazonAdsActionType
    AmazonAdActionCollapsed,
    AmazonAdActionDismissed,
} AmazonAdAction;

S3E_END_C_DECL

#endif
