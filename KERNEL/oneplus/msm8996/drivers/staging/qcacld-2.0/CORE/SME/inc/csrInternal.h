/*
 * Copyright (c) 2011-2019 The Linux Foundation. All rights reserved.
 *
 * Previously licensed under the ISC license by Qualcomm Atheros, Inc.
 *
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * This file was originally distributed by Qualcomm Atheros, Inc.
 * under proprietary terms before Copyright ownership was assigned
 * to the Linux Foundation.
 */

/** ------------------------------------------------------------------------- *
    ------------------------------------------------------------------------- *


    \file csrInternal.h

    Define internal data structure for MAC.
========================================================================== */
#ifndef CSRINTERNAL_H__
#define CSRINTERNAL_H__

#include "vos_status.h"
#include "vos_lock.h"

#include "vos_timer.h"
#include "csrSupport.h"
#include "vos_nvitem.h"
#include "wlan_qct_tl.h"
#include "vos_utils.h"

#include "csrApi.h"

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
#include "csrNeighborRoam.h"
#endif

#define CSR_MAX_STA (HAL_NUM_STA)

//define scan return criteria. LIM should use these define as well
#define CSR_SCAN_RETURN_AFTER_ALL_CHANNELS          (    0 )
#define CSR_SCAN_RETURN_AFTER_FIRST_MATCH           ( 0x01 )
#define CSR_SCAN_RETURN_AFTER_5_BAND_11d_FOUND      ( 0x80 )
#define CSR_SCAN_RETURN_AFTER_24_BAND_11d_FOUND     ( 0x40 )
#define CSR_SCAN_RETURN_AFTER_EITHER_BAND_11d_FOUND ( CSR_SCAN_RETURN_AFTER_5_BAND_11d_FOUND | CSR_SCAN_RETURN_AFTER_24_BAND_11d_FOUND )
#define CSR_NUM_RSSI_CAT        15
#define CSR_ROAM_SCAN_CHANNEL_SWITCH_TIME        3

//Support for multiple session
#define CSR_SESSION_ID_INVALID    0xFF   // session ID invalid
#define CSR_ROAM_SESSION_MAX      5   // No of sessions to be supported, and a
                                      // session is for Infra, IBSS or BT-AMP
#define CSR_SESSION_ID_ANY        50

#define CSR_IS_SESSION_VALID( pMac, sessionId ) ( ( (sessionId) < CSR_ROAM_SESSION_MAX ) \
                                                  && ( (pMac)->roam.roamSession[(sessionId)].sessionActive ) )
#define CSR_GET_SESSION( pMac, sessionId ) \
( \
    (sessionId < CSR_ROAM_SESSION_MAX) ? \
     (&(pMac)->roam.roamSession[(sessionId)]) :\
     NULL \
)

#define CSR_IS_SESSION_ANY(sessionId) (sessionId == CSR_SESSION_ID_ANY)
#define CSR_MAX_NUM_COUNTRY_CODE  100
#define CSR_IS_SELECT_5GHZ_MARGIN( pMac ) \
( \
   (((pMac)->roam.configParam.nSelect5GHzMargin)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)
#define CSR_IS_SELECT_5G_PREFERRED(pMac) \
( \
   (((pMac)->roam.configParam.roam_params.is_5g_pref_enabled)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_ESE) || defined(FEATURE_WLAN_LFR)
#define CSR_IS_ROAM_PREFER_5GHZ( pMac ) \
( \
   (((pMac)->roam.configParam.nRoamPrefer5GHz)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)
#define CSR_IS_ROAM_INTRA_BAND_ENABLED( pMac ) \
( \
   (((pMac)->roam.configParam.nRoamIntraBand)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)
#endif

#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
#define CSR_IS_ROAM_SCAN_OFFLOAD_ENABLED( pMac ) \
( \
   (((pMac)->roam.configParam.isRoamOffloadScanEnabled)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)

#define CSR_IS_FASTROAM_IN_CONCURRENCY_INI_FEATURE_ENABLED( pMac ) \
( \
   (((pMac)->roam.configParam.bFastRoamInConIniFeatureEnabled)?eANI_BOOLEAN_TRUE:eANI_BOOLEAN_FALSE) \
)
#endif

//Support for "Fast roaming" (i.e., ESE, LFR, or 802.11r.)
#define CSR_BG_SCAN_OCCUPIED_CHANNEL_LIST_LEN 15

typedef enum
{
    eCSR_CFG_DOT11_MODE_ABG,
    eCSR_CFG_DOT11_MODE_11A,
    eCSR_CFG_DOT11_MODE_11B,
    eCSR_CFG_DOT11_MODE_11G,
    eCSR_CFG_DOT11_MODE_11N,
#ifdef WLAN_FEATURE_11AC
    eCSR_CFG_DOT11_MODE_11AC,
#endif
    eCSR_CFG_DOT11_MODE_11G_ONLY,
    eCSR_CFG_DOT11_MODE_11N_ONLY,
#ifdef WLAN_FEATURE_11AC
    eCSR_CFG_DOT11_MODE_11AC_ONLY,
#endif
    //This value can never set to CFG. It is for CSR's internal use
    eCSR_CFG_DOT11_MODE_AUTO,
}eCsrCfgDot11Mode;  //Used to determine what to set to the WNI_CFG_DOT11_MODE

typedef enum etCsrRoamCommands
{
    eCsrRoamNoCommand,
    eCsrRoamCommandScan,
    eCsrRoamCommandRoam,
    eCsrRoamCommandWmStatusChange,
    eCsrRoamCommandSetKey,
    eCsrRoamCommandRemoveKey,

} eCsrRoamCommands;

typedef enum
{
    eCsrScanOther = 1,
    eCsrScanLostLink1,
    eCsrScanLostLink2,
    eCsrScanLostLink3,
    eCsrScanLostLink4,
    eCsrScan11d1,  //First 11d scan
    eCsrScan11d2,  //First 11d scan has failed
    eCsrScan11dDone,  //11d scan succeeded, try the rest of the channel
    eCsrScanUserRequest,
    eCsrScanGetResult,
    eCsrScanSetBGScanParam, //used for HO too - bg scan request in NT Handoff sub-state
    eCsrScanForSsid,
    eCsrScanForCapsChange,
    eCsrScanBGScanAbort,
    eCsrScanBGScanEnable,
    eCsrScanIdleScan,
    eCsrScanGetScanChnInfo,     //To get the list of channels scanned

    eCsrScanBgScan, // bg scan request in NRT & RT Handoff sub-states
    eCsrScanProbeBss, // directed probe on an entry from the candidate list - HO
    eCsrScanAbortBgScan,    //aborting a BG scan (meaning the scan is triggered by LIM timer)
    eCsrScanAbortNormalScan, //aborting a normal scan (the scan is trigger by eWNI_SME_SCAN_REQ)
    eCsrScanP2PFindPeer,
    eCsrScanGetLfrResult, // get the LFR candidates from PE scan cache
}eCsrScanReason;

typedef enum
{
    eCsrNoConnection,          // Roaming because we have not established the initial connection.
    eCsrCapsChange,            // roaming because LIM reported a Capability change in the associated AP.
    eCsrForcedDisassoc,        // roaming becuase someone asked us to Disassoc and stay disassociated.
    eCsrHddIssued,             // roaming because an 802.11 request was issued to the driver.
    eCsrLostLink1,             // roaming because we lost link to an associated AP
    eCsrLostLink2,
    eCsrLostLink3,
    eCsrForcedDisassocMICFailure, // roaming because we need to force a Disassoc due to MIC failure
    eCsrHddIssuedReassocToSameAP,
    eCsrSmeIssuedReassocToSameAP,
    eCsrSmeIssuedReassocToDiffAP,
    eCsrForcedDeauth,        // roaming becuase someone asked us to deauth and stay disassociated.
    eCsrSmeIssuedDisassocForHandoff, // will be issued by Handoff logic to disconect from current AP
    eCsrSmeIssuedAssocToSimilarAP, // will be issued by Handoff logic to join a new AP with same profile
    eCsrSmeIssuedIbssJoinFailure, // ibss join timer fired before any perr showed up, so shut down the network
    eCsrForcedIbssLeave,
    eCsrStopBss,
    eCsrSmeIssuedFTReassoc,
    eCsrForcedDisassocSta,
    eCsrForcedDeauthSta,
    eCsrPerformPreauth,
    eCsrLostLink1Abort,
    eCsrLostLink2Abort,
    eCsrLostLink3Abort,
}eCsrRoamReason;

typedef enum
{
    eCSR_ROAM_SUBSTATE_NONE = 0,
    eCSR_ROAM_SUBSTATE_START_BSS_REQ,
    eCSR_ROAM_SUBSTATE_JOIN_REQ,
    eCSR_ROAM_SUBSTATE_REASSOC_REQ,
    eCSR_ROAM_SUBSTATE_DISASSOC_REQ,
    eCSR_ROAM_SUBSTATE_STOP_BSS_REQ,
    eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING,   //Continue the current roam command after disconnect
    eCSR_ROAM_SUBSTATE_AUTH_REQ,
    eCSR_ROAM_SUBSTATE_CONFIG,
    eCSR_ROAM_SUBSTATE_DEAUTH_REQ,
    eCSR_ROAM_SUBSTATE_DISASSOC_NOTHING_TO_JOIN,
    eCSR_ROAM_SUBSTATE_DISASSOC_REASSOC_FAILURE,
    eCSR_ROAM_SUBSTATE_DISASSOC_FORCED,
    eCSR_ROAM_SUBSTATE_WAIT_FOR_KEY,
    eCSR_ROAM_SUBSTATE_DISASSOC_HANDOFF,
    eCSR_ROAM_SUBSTATE_JOINED_NO_TRAFFIC,
    eCSR_ROAM_SUBSTATE_JOINED_NON_REALTIME_TRAFFIC,
    eCSR_ROAM_SUBSTATE_JOINED_REALTIME_TRAFFIC,
    eCSR_ROAM_SUBSTATE_DISASSOC_STA_HAS_LEFT,
//  max is 15 unless the bitfield is expanded...
} eCsrRoamSubState;


typedef enum
{
  eCSR_ROAMING_STATE_STOP = 0,
  eCSR_ROAMING_STATE_IDLE,
  eCSR_ROAMING_STATE_SCANNING,
  eCSR_ROAMING_STATE_JOINING,
  eCSR_ROAMING_STATE_JOINED,
}eCsrRoamState;


typedef enum
{
    eCsrContinueRoaming,
    eCsrStopRoaming,
    eCsrStartIbss,
    eCsrStartIbssSameIbss,
    eCsrReassocToSelfNoCapChange,
    eCsrStopRoamingDueToConcurrency,

}eCsrJoinState;

typedef enum
{
    eCsrNotRoaming,
    eCsrLostlinkRoamingDisassoc,
    eCsrLostlinkRoamingDeauth,
    eCsrDynamicRoaming,
   eCsrReassocRoaming,
}eCsrRoamingReason;

typedef enum
{
    eCsrDisassociated,
    eCsrDeauthenticated

}eCsrRoamWmStatusChangeTypes;

typedef enum
{
   eCsrSummaryStats = 0,
   eCsrGlobalClassAStats,
   eCsrGlobalClassBStats,
   eCsrGlobalClassCStats,
   eCsrGlobalClassDStats,
   eCsrPerStaStats,
   csr_per_chain_rssi_stats,
   eCsrMaxStats
}eCsrRoamStatsClassTypes;

#ifdef FEATURE_WLAN_DIAG_SUPPORT
typedef enum
{
   eCSR_WLAN_STATUS_CONNECT =0,
   eCSR_WLAN_STATUS_DISCONNECT

}eCsrDiagWlanStatusEventSubtype;

typedef enum
{
   eCSR_REASON_UNSPECIFIED = 0,
   eCSR_REASON_USER_REQUESTED,
   eCSR_REASON_MIC_ERROR,
   eCSR_REASON_DISASSOC,
   eCSR_REASON_DEAUTH,
   eCSR_REASON_HANDOFF,

}eCsrDiagWlanStatusEventReason;

typedef enum
{
    eCSR_EVENT_TYPE_INVALID = 0,
    eCSR_EVENT_SCAN_COMPLETE = 72,
    eCSR_EVENT_SCAN_RES_FOUND = 73,
} eCSR_WLAN_DIAG_EVENT_TYPE;

#endif //FEATURE_WLAN_DIAG_SUPPORT

typedef struct tagCsrChannel
{
    tANI_U8 numChannels;
    tANI_U8 channelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];
}tCsrChannel;

typedef struct tagScanProfile
{
    tANI_U32 minChnTime;
    tANI_U32 maxChnTime;
    /* In units of milliseconds, ignored when not connected */
    tANI_U32 restTime;  //This is ignored if not associated
    /* In units of milliseconds, ignored when not connected */
    tANI_U32 min_rest_time;
    /* In units of milliseconds, ignored when not connected */
    tANI_U32 idle_time;
    tANI_U32 numOfChannels;
    tANI_U8 *pChannelList;
    tSirScanType scanType;  //active or passive
    eCsrRoamBssType bssType;    //BSS or IBSS
    tANI_U8 ssid[WNI_CFG_SSID_LEN];
    tANI_U8 bReturnAfter1stMatch;
    tANI_U8 fUniqueResult;
    tANI_U8 freshScan;
    tCsrBssid bssid;
}tScanProfile;

typedef struct tagBssConfigParam
{
    eCsrMediaAccessType qosType;
    tSirMacSSid SSID;
    tANI_U32 uRTSThresh;
    tANI_U32 uDeferThresh;  //
    eCsrCfgDot11Mode uCfgDot11Mode;
    eCsrBand eBand;
    tANI_U8 standardRate[CSR_DOT11_SUPPORTED_RATES_MAX];
    tANI_U8 extendedRate[CSR_DOT11_EXTENDED_SUPPORTED_RATES_MAX];
    eCsrExposedTxRate txRate;
    tAniAuthType authType;
    eCsrEncryptionType encType;
    tANI_U32 uShortSlotTime;
    tANI_U32 uHTSupport;    //High throughput
    tANI_U32 uPowerLimit;
    tANI_U32 uHeartBeatThresh;
    tANI_U32 uJoinTimeOut;
    tSirMacCapabilityInfo BssCap;
    tANI_BOOLEAN f11hSupport;
    ePhyChanBondState cbMode;
}tBssConfigParam;


typedef struct tagCsrRoamStartBssParams
{
    tSirMacSSid         ssId;
    tCsrBssid           bssid;    //this is the BSSID for the party we want to join (only use for IBSS or WDS)
    tSirNwType          sirNwType;
    ePhyChanBondState   cbMode;
    tSirMacRateSet      operationalRateSet;
    tSirMacRateSet      extendedRateSet;
    tANI_U8             operationChn;
    tANI_U8             vht_channel_width;
    eCsrCfgDot11Mode    uCfgDot11Mode;
    tANI_U8             privacy;
    tANI_BOOLEAN        fwdWPSPBCProbeReq;
    tANI_BOOLEAN        protEnabled;
    tANI_BOOLEAN        obssProtEnabled;
    tAniAuthType        authType;
    tANI_U16            beaconInterval;    //If this is 0, SME will fill in for caller.
    tANI_U16            ht_protection;
    tANI_U32            dtimPeriod;
    tANI_U8             ApUapsdEnable;
    tANI_U8             ssidHidden;
    tANI_U8             wps_state;
    tVOS_CON_MODE       bssPersona;
    tANI_U16            nRSNIELength;  //The byte count in the pRSNIE, if 0, pRSNIE is ignored.
    tANI_U8             *pRSNIE;     //If not null, it has the IE byte stream for RSN
    tANI_BOOLEAN        updatebeaconInterval; //Flag used to indicate update
                                             // beaconInterval
#ifdef WLAN_FEATURE_11W
    tANI_BOOLEAN        mfpCapable;
    tANI_BOOLEAN        mfpRequired;
#endif

    tSirAddIeParams     addIeParams;
    uint8_t             sap_dot11mc;
    uint16_t            beacon_tx_rate;
}tCsrRoamStartBssParams;


typedef struct tagScanCmd
{
    tANI_U32                scanID;
    csrScanCompleteCallback callback;
    void                    *pContext;
    eCsrScanReason          reason;
    tCsrRoamProfile         *pToRoamProfile;
    tANI_U32                roamId;    //this is the ID related to the pToRoamProfile
    union
    {
        tCsrScanRequest   scanRequest;
        tCsrBGScanRequest bgScanRequest;
    }u;
    //This flag will be set while aborting the scan due to band change
     eCsrAbortReason        abort_scan_indication;
}tScanCmd;

typedef struct tagRoamCmd
{
    tANI_U32 roamId;
    eCsrRoamReason roamReason;
    tCsrRoamProfile roamProfile;
    tScanResultHandle hBSSList;     //BSS list fits the profile
    tListElem *pRoamBssEntry;  //point to the current BSS in the list that is roaming. It starts from head to tail
    tSirBssDescription *pLastRoamBss;   //the last BSS we try and failed
    tANI_BOOLEAN fReleaseBssList;  //whether to free hBSSList
    tANI_BOOLEAN fReleaseProfile;  //whether to free roamProfile
    tANI_BOOLEAN fReassoc;  //whether this command is for reassociation
    tANI_BOOLEAN fUpdateCurRoamProfile;     //whether pMac->roam.pCurRoamProfile needs to be updated
    //this is for CSR internal used only. And it should not be assigned when creating the command
    //This causes the roam command not to do anything.
    tANI_BOOLEAN fReassocToSelfNoCapChange;

    tANI_BOOLEAN fStopWds;
    tSirMacAddr peerMac;
    tSirMacReasonCodes reason;
}tRoamCmd;

typedef struct tagSetKeyCmd
{
    tANI_U32 roamId;
    eCsrEncryptionType encType;
    eCsrAuthType authType;
    tAniKeyDirection keyDirection;    //Tx, Rx or Tx-and-Rx
    tSirMacAddr peerMac;   //Peer's MAC address. ALL 1's for group key
    tANI_U8 paeRole;      //0 for supplicant
    tANI_U8 keyId;  // Kye index
    tANI_U8 keyLength;  //Number of bytes containing the key in pKey
    tANI_U8 Key[CSR_MAX_KEY_LEN];
    tANI_U8 keyRsc[CSR_MAX_RSC_LEN];
} tSetKeyCmd;

typedef struct tahRemoveKeyCmd
{
    tANI_U32 roamId;
    eCsrEncryptionType encType;
    eCsrAuthType authType;
    tSirMacAddr peerMac;   //Peer's MAC address. ALL 1's for group key
    tANI_U8 keyId;  //key index
} tRemoveKeyCmd;

typedef struct tagWmStatusChangeCmd
{
    eCsrRoamWmStatusChangeTypes Type;
    union
    {
        tSirSmeDeauthInd   DeauthIndMsg;
        tSirSmeDisassocInd DisassocIndMsg;
    }u;

}tWmStatusChangeCmd;


typedef struct tagAddStaForSessionCmd
{
   //Session self mac addr
   tSirMacAddr selfMacAddr;
   tVOS_CON_MODE currDeviceMode;
   tANI_U32        type;
   tANI_U32        subType;
   tANI_U8         sessionId;
}tAddStaForSessionCmd;

typedef struct tagDelStaForSessionCmd
{
   //Session self mac addr
   tSirMacAddr selfMacAddr;
   csrRoamSessionCloseCallback callback;
   void *pContext;
}tDelStaForSessionCmd;

//This structure represents one scan request
typedef struct tagCsrCmd
{
    tListElem Link;
    eCsrRoamCommands command;
    tANI_U8 sessionId;              // Session ID for this command
    union
    {
        tScanCmd scanCmd;
        tRoamCmd roamCmd;
        tWmStatusChangeCmd wmStatusChangeCmd;
        tSetKeyCmd setKeyCmd;
        tRemoveKeyCmd removeKeyCmd;
        tAddStaForSessionCmd addStaSessionCmd;
        tDelStaForSessionCmd delStaSessionCmd;
    }u;
}tCsrCmd;

#ifdef WLAN_FEATURE_VOWIFI_11R
typedef struct tagCsr11rConfig
{
    tANI_BOOLEAN   IsFTResourceReqSupported;
} tCsr11rConfig;
#endif

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
typedef struct tagCsrNeighborRoamConfig
{
    tANI_U32       nNeighborScanTimerPeriod;
    tANI_U8        nNeighborLookupRssiThreshold;
    tANI_U8        nNeighborReassocRssiThreshold;
    tANI_U16       nNeighborScanMinChanTime;
    tANI_U16       nNeighborScanMaxChanTime;
    sCsrChannel    neighborScanChanList;
    tANI_U8        nMaxNeighborRetries;
    tANI_U16       nNeighborResultsRefreshPeriod;
    tANI_U16       nEmptyScanRefreshPeriod;
    tANI_U8        nOpportunisticThresholdDiff;
    tANI_U8        nRoamRescanRssiDiff;
    tANI_U8        nRoamBmissFirstBcnt;
    tANI_U8        nRoamBmissFinalBcnt;
    tANI_U8        nRoamBeaconRssiWeight;
    tANI_U8        delay_before_vdev_stop;
    uint32_t       nhi_rssi_scan_max_count;
    uint32_t       nhi_rssi_scan_rssi_delta;
    uint32_t       nhi_rssi_scan_delay;
    int32_t        nhi_rssi_scan_rssi_ub;
}tCsrNeighborRoamConfig;
#endif

typedef struct tagCsrConfig
{
    tANI_U32 agingCount;
    tANI_U32 FragmentationThreshold;
    tANI_U32 channelBondingMode24GHz;
    tANI_U32 channelBondingMode5GHz;
    uint32_t stacbmode;
    tANI_U32 RTSThreshold;
    eCsrPhyMode phyMode;
    eCsrCfgDot11Mode uCfgDot11Mode;
    eCsrBand eBand;
    tANI_U32 HeartbeatThresh50;
    tANI_U32 HeartbeatThresh24;
    tANI_U32 bgScanInterval;
    eCsrCBChoice cbChoice;
    eCsrBand bandCapability;     //indicate hw capability
    tANI_U8 gStaLocalEDCAEnable;
    eCsrRoamWmmUserModeType WMMSupportMode;
    tANI_BOOLEAN Is11eSupportEnabled;
    tANI_BOOLEAN Is11dSupportEnabled;
    tANI_BOOLEAN Is11dSupportEnabledOriginal;
    tANI_BOOLEAN Is11hSupportEnabled;
    tANI_BOOLEAN shortSlotTime;
    tANI_BOOLEAN ProprietaryRatesEnabled;
    tANI_BOOLEAN  fenableMCCMode;
    bool mcc_rts_cts_prot_enable;
    bool mcc_bcast_prob_resp_enable;
    tANI_U16 TxRate;
    tANI_U8  fAllowMCCGODiffBI;
    tANI_U8 AdHocChannel24;
    tANI_U8 AdHocChannel5G;
    tANI_U32 impsSleepTime;     //in units of microseconds
    tANI_U32 scanAgeTimeNCNPS;  //scan result aging time threshold when Not-Connect-No-Power-Save, in seconds
    tANI_U32 scanAgeTimeNCPS;   //scan result aging time threshold when Not-Connect-Power-Save, in seconds
    tANI_U32 scanAgeTimeCNPS;   //scan result aging time threshold when Connect-No-Power-Save, in seconds,
    tANI_U32 scanAgeTimeCPS;   //scan result aging time threshold when Connect-Power-Savein seconds
    tANI_U32 BssPreferValue[CSR_NUM_RSSI_CAT];  //each RSSI category has one value
    int RSSICat[CSR_NUM_RSSI_CAT];
    tANI_U8 bCatRssiOffset;     //to set the RSSI difference for each category
    tANI_U32 nRoamingTime;  //In seconds, CSR will try this long before gives up, 0 means no roaming
    //Whether to limit the channels to the ones set in Csr11dInfo. If true, the opertaional
    //channels are limited to the default channel list. It is an "AND" operation between the
    //default channels and the channels in the 802.11d IE.
    tANI_BOOLEAN fEnforce11dChannels;
    //Country Code Priority
    //0 = 802.11D > Configured Country > NV
    //1 = Configured Country > 802.11D > NV
    tANI_BOOLEAN fSupplicantCountryCodeHasPriority;
    //When true, AP with unknown country code won't be see.
    //"Unknown country code" means either Ap doesn't have 11d IE or we cannot
    //find a domain for the country code in its 11d IE.
    tANI_BOOLEAN fEnforceCountryCodeMatch;
    //When true, only APs in the default domain can be seen. If the Ap has "unknown country
    //code", or the doamin of the country code doesn't match the default domain, the Ap is
    //not acceptable.
    tANI_BOOLEAN fEnforceDefaultDomain;

    tANI_U16 vccRssiThreshold;
    tANI_U32 vccUlMacLossThreshold;

    tANI_U32  nPassiveMinChnTime;    //in units of milliseconds
    tANI_U32  nPassiveMaxChnTime;    //in units of milliseconds
    tANI_U32  nActiveMinChnTime;     //in units of milliseconds
    tANI_U32  nActiveMaxChnTime;     //in units of milliseconds

    tANI_U32  nInitialDwellTime;     //in units of milliseconds
    bool      initial_scan_no_dfs_chnl;

    tANI_U8   disableAggWithBtc;
#ifdef WLAN_AP_STA_CONCURRENCY
    tANI_U32  nPassiveMinChnTimeConc;    //in units of milliseconds
    tANI_U32  nPassiveMaxChnTimeConc;    //in units of milliseconds
    tANI_U32  nActiveMinChnTimeConc;     //in units of milliseconds
    tANI_U32  nActiveMaxChnTimeConc;     //in units of milliseconds
    /* In units of milliseconds */
    tANI_U32  nRestTimeConc;
    /* In units of milliseconds */
    tANI_U32  min_rest_time_conc;
    /* In units of milliseconds */
    tANI_U32  idle_time_conc;

#endif

    tANI_BOOLEAN IsIdleScanEnabled;
    //in dBm, the maximum TX power
    //The actual TX power is the lesser of this value and 11d.
    //If 11d is disable, the lesser of this and default setting.
    tANI_U8 nTxPowerCap;
    tANI_BOOLEAN allow_tpc_from_ap;
    tANI_U32  statsReqPeriodicity;  //stats request frequency from PE while in full power
    tANI_U32  statsReqPeriodicityInPS;//stats request frequency from PE while in power save
    tANI_U32 dtimPeriod;
    tANI_BOOLEAN    ssidHidden;

#ifdef WLAN_FEATURE_VOWIFI_11R
    tCsr11rConfig csr11rConfig;
#endif

#ifdef FEATURE_WLAN_LFR
    tANI_U8      isFastRoamIniFeatureEnabled;
    tANI_U8      MAWCEnabled;
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
    tANI_U8      isRoamOffloadScanEnabled;
    tANI_BOOLEAN bFastRoamInConIniFeatureEnabled;
#endif
#endif

#ifdef FEATURE_WLAN_ESE
    tANI_U8   isEseIniFeatureEnabled;
#endif

#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_ESE) || defined(FEATURE_WLAN_LFR)
    tANI_U8       isFastTransitionEnabled;
    tANI_U8       RoamRssiDiff;
    tANI_U8       nImmediateRoamRssiDiff;
    tANI_BOOLEAN  nRoamPrefer5GHz;
    tANI_BOOLEAN  nRoamIntraBand;
    tANI_BOOLEAN  isWESModeEnabled;
    tANI_BOOLEAN  nRoamScanControl;
    tANI_U8       nProbes;
    tANI_U16      nRoamScanHomeAwayTime;
#endif

#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    tCsrNeighborRoamConfig neighborRoamConfig;
#endif

    /* Instead of Reassoc, send ADDTS/DELTS even when ACM is off for that AC
     * This is mandated by WMM-AC certification */
    tANI_BOOLEAN addTSWhenACMIsOff;

    tANI_BOOLEAN fValidateList;
    //Remove this code once SLM_Sessionization is supported
    //BMPS_WORKAROUND_NOT_NEEDED
    tANI_BOOLEAN doBMPSWorkaround;

    //To enable/disable scanning 2.4Ghz channels twice on a single scan request from HDD
    tANI_BOOLEAN fScanTwice;
#ifdef WLAN_FEATURE_11AC
    tANI_U32  nVhtChannelWidth;
    tANI_U8   txBFEnable;
    tANI_U8   txBFCsnValue;
    tANI_U8   enable2x2;
    tANI_BOOLEAN enableVhtFor24GHz;
    tANI_U8   txMuBformee;
    tANI_U8   enableVhtpAid;
    tANI_U8   enableVhtGid;
#endif
    tANI_U8   enableAmpduPs;
    tANI_U8   enableHtSmps;
    tANI_U8   htSmps;
    tANI_U8   txLdpcEnable;

    /*
     * Enable/Disable heartbeat offload
     */
    tANI_BOOLEAN enableHeartBeatOffload;
    tANI_U8 max_amsdu_num;
    tANI_U8 nSelect5GHzMargin;
    tANI_U8 isCoalesingInIBSSAllowed;
#ifdef FEATURE_WLAN_MCC_TO_SCC_SWITCH
    tANI_U8 cc_switch_mode;
    bool    band_switch_enable;
    bool    ap_p2pgo_concurrency_enable;
    bool    ap_p2pclient_concur_enable;
#endif
    tANI_U8 allowDFSChannelRoam;
#ifdef WLAN_FEATURE_ROAM_OFFLOAD
    tANI_BOOLEAN  isRoamOffloadEnabled;
#endif
    tANI_BOOLEAN obssEnabled;
    v_U8_t conc_custom_rule1;
    v_U8_t conc_custom_rule2;
    v_U8_t is_sta_connection_in_5gz_enabled;
    struct roam_ext_params roam_params;
    tANI_BOOLEAN sendDeauthBeforeCon;
    tANI_BOOLEAN ignorePeerErpInfo;
    bool ignore_peer_ht_opmode;
    v_U16_t pkt_err_disconn_th;
    bool enable_fatal_event;
    uint32_t tx_aggregation_size;
    uint32_t rx_aggregation_size;
    uint32_t tx_aggr_sw_retry_threshhold_be;
    uint32_t tx_aggr_sw_retry_threshhold_bk;
    uint32_t tx_aggr_sw_retry_threshhold_vi;
    uint32_t tx_aggr_sw_retry_threshhold_vo;
    uint32_t tx_non_aggr_sw_retry_threshhold_be;
    uint32_t tx_non_aggr_sw_retry_threshhold_bk;
    uint32_t tx_non_aggr_sw_retry_threshhold_vi;
    uint32_t tx_non_aggr_sw_retry_threshhold_vo;
    bool enable_edca_params;
    uint32_t edca_vo_cwmin;
    uint32_t edca_vi_cwmin;
    uint32_t edca_bk_cwmin;
    uint32_t edca_be_cwmin;
    uint32_t edca_vo_cwmax;
    uint32_t edca_vi_cwmax;
    uint32_t edca_bk_cwmax;
    uint32_t edca_be_cwmax;
    uint32_t edca_vo_aifs;
    uint32_t edca_vi_aifs;
    uint32_t edca_bk_aifs;
    uint32_t edca_be_aifs;
    bool vendor_vht_for_24ghz_sap;
    struct csr_sta_roam_policy_params sta_roam_policy;
    bool enable_bcast_probe_rsp;
    uint16_t wlm_latency_enable;
    uint16_t wlm_latency_level;
    uint32_t wlm_latency_flags[CSR_NUM_WLM_LATENCY_LEVEL];
}tCsrConfig;

typedef struct tagCsrChannelPowerInfo
{
    tListElem link;
    tANI_U8 firstChannel;
    tANI_U8 numChannels;
    tANI_U8 txPower;
    tANI_U8 interChannelOffset;
}tCsrChannelPowerInfo;

typedef struct tagRoamJoinStatus
{
    tSirResultCodes statusCode;
    //this is set to unspecified if statusCode indicates timeout. Or it is the failed reason from the other BSS(per 802.11 spec)
    tANI_U32 reasonCode;
    tSirMacAddr bssId;
}tCsrRoamJoinStatus;

typedef struct tagCsrOsChannelMask
{
    tANI_U8 numChannels;
    tANI_BOOLEAN scanEnabled[WNI_CFG_VALID_CHANNEL_LIST_LEN];
    tANI_U8 channelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];
}tCsrOsChannelMask;

typedef struct tagCsrVotes11d
{
    tANI_U8 votes;
    tANI_U8 countryCode[WNI_CFG_COUNTRY_CODE_LEN];
}tCsrVotes11d;

typedef struct tagCsrScanStruct
{
    tScanProfile scanProfile;
    tANI_U32 nextScanID;
    tDblLinkList scanResultList;
    tDblLinkList tempScanResults;
    tANI_BOOLEAN fScanEnable;
    tANI_BOOLEAN fFullScanIssued;
    vos_timer_t hTimerGetResult;
    vos_timer_t hTimerIdleScan;
    //changes on every scan, it is used as a flag for whether 11d info is found on every scan
    tANI_U8 channelOf11dInfo;
    tANI_U8 scanResultCfgAgingTime;
    //changes on every scan, a flag to tell whether conflict 11d info found on each BSS
    tANI_BOOLEAN fAmbiguous11dInfoFound;
    //changes on every scan, a flag to tell whether the applied 11d info present in one of the scan results
    tANI_BOOLEAN fCurrent11dInfoMatch;
    tANI_BOOLEAN f11dInfoReset;     //to indicate whether the 11d info in CFG is reset to default
    tSirScanType curScanType;
    tCsrChannel baseChannels;   //This are all the supported channels AND(&) to the current eBand
    tCsrChannel channels11d;
    tChannelListWithPower defaultPowerTable[WNI_CFG_VALID_CHANNEL_LIST_LEN]; //From NV
    tChannelListWithPower defaultPowerTable40MHz[WNI_CFG_VALID_CHANNEL_LIST_LEN]; //From NV
    tANI_U32 numChannelsDefault; //total channels of NV
    tCsrChannel base20MHzChannels;   //The channel base to work on
    tCsrChannel base40MHzChannels;   //center channels for 40MHz channels
    tDblLinkList channelPowerInfoList24;
    tDblLinkList channelPowerInfoList5G;
    tANI_U32 nLastAgeTimeOut;
    tANI_U32 nAgingCountDown;
    tANI_U8 countryCodeDefault[WNI_CFG_COUNTRY_CODE_LEN];     //The country code from NV
    tANI_U8 countryCodeCurrent[WNI_CFG_COUNTRY_CODE_LEN];
    tANI_U8 countryCode11d[WNI_CFG_COUNTRY_CODE_LEN];
    v_REGDOMAIN_t domainIdDefault;  //default regulatory domain
    v_REGDOMAIN_t domainIdCurrent;  //current regulatory domain
    tCsrBssid currentCountryBssid;  // Bssid for current country code
    tANI_S8 currentCountryRSSI;     // RSSI for current country code
    tANI_BOOLEAN f11dInfoApplied;
    tANI_BOOLEAN fCancelIdleScan;
    tANI_U8 countryCodeCount;
    tCsrVotes11d votes11d[CSR_MAX_NUM_COUNTRY_CODE]; //counts for various advertized country codes
    //in 11d IE from probe rsp or beacons of neighboring APs;
    //will use the most popular one (max count)
    tANI_U8 countryCodeElected[WNI_CFG_COUNTRY_CODE_LEN];

    tANI_U8 numBGScanChannel;   //number of valid channels in the bgScanChannelList
    tANI_U8 bgScanChannelList[WNI_CFG_BG_SCAN_CHANNEL_LIST_LEN];
    //the ChannelInfo member is not used in this structure.
    //numBGScanChannel and bgScanChannelList are used for the BG scan channel info
    tCsrBGScanRequest bgScanParams;
    tANI_BOOLEAN fRestartIdleScan;
    tANI_U32 nIdleScanTimeGap;  //the time since last trying to trigger idle scan
    tCsrOsChannelMask osScanChannelMask;//keep a track of channels to be scnned while in traffic condition
    tANI_U16 nBssLimit; //the maximum number of BSS in scan cache
    /*channelPowerInfoList24 has been seen corrupted. Set this flag to true trying to
    * detect when it happens. Adding this into code because we can't reproduce it easily.
    * We don't know when it happens. */
    tANI_BOOLEAN fValidateList;
    /*Customer wants to start with an active scan based on the default country code.
    * This optimization will minimize the driver load to association time.
    * Based on this flag we will bypass the initial passive scan needed for 11d
    * to determine the country code & domain */
    tANI_BOOLEAN fEnableBypass11d;

    /*Customer wants to optimize the scan time. Avoiding scans(passive) on DFS
    * channels while swipping through both bands can save some time
    * (apprx 1.3 sec) */
    tANI_U8 fEnableDFSChnlScan;

    /*
    * To enable/disable scanning only 2.4Ghz channels on first scan
    */
    tANI_BOOLEAN fFirstScanOnly2GChnl;

    tANI_BOOLEAN fDropScanCmd; //true means we don't accept scan commands

#ifdef WLAN_AP_STA_CONCURRENCY
    tDblLinkList scanCmdPendingList;
#endif
    /* This includes all channels on which candidate APs are found */
    tCsrChannel occupiedChannels[CSR_ROAM_SESSION_MAX];
    tANI_S8     inScanResultBestAPRssi;
    eCsrBand  scanBandPreference;  //This defines the band perference for scan
    csrScanCompleteCallback callback11dScanDone;
    bool fcc_constraint;
    bool defer_update_channel_list;
}tCsrScanStruct;

//Save the connected information. This structure + connectedProfile
//should contain all information about the connection
typedef struct tagRoamCsrConnectedInfo
{
    tANI_U32 nBeaconLength; //the length, in bytes, of the beacon frame, can be 0
    tANI_U32 nAssocReqLength;   //the length, in bytes, of the assoc req frame, can be 0
    tANI_U32 nAssocRspLength;   //The length, in bytes, of the assoc rsp frame, can be 0
#ifdef WLAN_FEATURE_VOWIFI_11R
    tANI_U32 nRICRspLength; //Length of the parsed RIC response IEs received in reassoc response
#endif
#ifdef FEATURE_WLAN_ESE
    tANI_U32 nTspecIeLength;
#endif
    tANI_U8 *pbFrames;  //Point to a buffer contain the beacon, assoc req, assoc rsp frame, in that order
                        //user needs to use nBeaconLength, nAssocReqLength, nAssocRspLength to desice where
                        //each frame starts and ends.
    tANI_U8 staId;
}tCsrRoamConnectedInfo;


typedef struct tagCsrLinkQualityIndInfo
{
   csrRoamLinkQualityIndCallback  callback;
   void                          *context;
}tCsrLinkQualityIndInfo;

typedef struct tagCsrPeStatsReqInfo
{
   tListElem              link;  /* list links */
   tANI_U32               statsMask;
   tANI_U32               periodicity;
   tANI_BOOLEAN           rspPending;
   vos_timer_t            hPeStatsTimer;
   tANI_BOOLEAN           timerRunning;
   tANI_U8                staId;
   tANI_U8                numClient;
   tpAniSirGlobal         pMac;
   /* To remember if the peStats timer is stopped successfully or not */
   tANI_BOOLEAN           timerStopFailed;
   tANI_U8                sessionId;

}tCsrPeStatsReqInfo;

typedef struct tagCsrStatsClientReqInfo
{
   tListElem              link;  /* list links */
   eCsrStatsRequesterType requesterId;
   tCsrStatsCallback      callback;
   tANI_U32               periodicity;
   void                  *pContext;
   tANI_U32               statsMask;
   tCsrPeStatsReqInfo    *pPeStaEntry;
   tANI_U8                staId;
   vos_timer_t            timer;
   tANI_BOOLEAN           timerExpired;
   tpAniSirGlobal         pMac; // TODO: Confirm this change BTAMP
   tANI_U8                sessionId;
}tCsrStatsClientReqInfo;

typedef struct tagCsrTlStatsReqInfo
{
   tANI_U32               periodicity;
   tANI_BOOLEAN           timerRunning;
   vos_timer_t            hTlStatsTimer;
   tANI_U8                numClient;
}tCsrTlStatsReqInfo;

#ifdef WLAN_FEATURE_ROAM_OFFLOAD
typedef enum
{

    /* reassociation is done but couldn't finish security handshake */
    eSIR_ROAM_AUTH_STATUS_CONNECTED = 1,

    /* roam successfully completed by firmware */
    eSIR_ROAM_AUTH_STATUS_AUTHENTICATED = 2,

    /* unknown error */
    eSIR_ROAM_AUTH_STATUS_UNKNOWN = 0xff

} tCsrRoamOffloadAuthStatus;
typedef struct tagCsrRoamOffloadSynchStruct
{
    tANI_U8 roamedVdevId; /* vdevId after roaming */
    tANI_S8 txMgmtPower;  /* HAL fills in the tx power used for */
    tANI_U8 rssi;  /* RSSI */
    tANI_U8 roamReason;  /* Roam reason */
    tANI_U8 nss;  /* no of spatial streams */
    tANI_U16 chainMask;  /* chainmask */
    tANI_U16 smpsMode;  /* smps.mode */
    tSirMacAddr bssid;  /* MAC address of roamed AP */
    tANI_BOOLEAN bRoamSynchInProgress; /* a roam offload synch*/
    tCsrRoamOffloadAuthStatus authStatus;  /* authentication
                                              status */
    tANI_U8 kck[SIR_KCK_KEY_LEN];
    tANI_U8 kek[SIR_KEK_KEY_LEN];
    tANI_U8 replay_ctr[SIR_REPLAY_CTR_LEN];
    tpSirBssDescription  pbssDescription; /*BSS descriptor*/
} tCsrRoamOffloadSynchStruct;
#endif

typedef struct tagCsrRoamStoredProfile
{
    tANI_U32 session_id;
    tCsrRoamProfile profile;
    tScanResultHandle bsslist_handle;
    eCsrRoamReason reason;
    tANI_U32 roam_id;
    tANI_BOOLEAN imediate_flag;
    tANI_BOOLEAN clear_flag;
} tCsrRoamStoredProfile;

typedef struct tagCsrRoamSession
{
    tANI_U8 sessionId;             // Session ID
    tANI_BOOLEAN sessionActive;      // TRUE if it is used
    tCsrBssid selfMacAddr;           // For BT-AMP station, this serve as BSSID for self-BSS.
    csrRoamCompleteCallback callback;
    void *pContext;
    eCsrConnectState connectState;
    tCsrRoamConnectedProfile connectedProfile;
    tCsrRoamConnectedInfo connectedInfo;
    tCsrRoamProfile *pCurRoamProfile;
    tSirBssDescription *pConnectBssDesc;
    tANI_U16 NumPmkidCache; /* valid no. of pmkid in the cache */
    uint16_t curr_cache_idx; /* the index in pmkidcache to write next to */
    tPmkidCacheInfo PmkidCacheInfo[CSR_MAX_PMKID_ALLOWED];
    tANI_U8 cJoinAttemps;
    //This may or may not have the up-to-date valid channel list
    //It is used to get WNI_CFG_VALID_CHANNEL_LIST and not allocate memory all the time
    tSirMacChanNum validChannelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];
    tANI_S32 sPendingCommands;    //0 means CSR is ok to low power
#ifdef FEATURE_WLAN_WAPI
    tANI_U16 NumBkidCache;
    tBkidCacheInfo BkidCacheInfo[CSR_MAX_BKID_ALLOWED];
#endif /* FEATURE_WLAN_WAPI */
    tANI_BOOLEAN fRoaming;  //indicate whether CSR is roaming (either via lostlink or dynamic roaming)
    //to remember some parameters needed for START_BSS.
    //All member must be set every time we try to join or start an IBSS or BT-AMP
    tCsrRoamStartBssParams bssParams;
    tANI_U32 nWpaRsnReqIeLength;   //the byte count of pWpaRsnIE;
    tANI_U8 *pWpaRsnReqIE; //this contain the WPA/RSN IE in assoc request or the one sent in beacon (IBSS)
    tANI_U32 nWpaRsnRspIeLength;    //the byte count for pWpaRsnRspIE
    tANI_U8 *pWpaRsnRspIE;  //this contain the WPA/RSN IE in beacon/probe rsp
#ifdef FEATURE_WLAN_WAPI
    tANI_U32 nWapiReqIeLength;   //the byte count of pWapiReqIE;
    tANI_U8 *pWapiReqIE; //this contain the WAPI IE in assoc request or the one sent in beacon (IBSS)
    tANI_U32 nWapiRspIeLength;    //the byte count for pWapiRspIE
    tANI_U8 *pWapiRspIE;  //this contain the WAPI IE in beacon/probe rsp
#endif /* FEATURE_WLAN_WAPI */
    tANI_U32 nAddIEScanLength;  //the byte count of pAddIeScanIE;
    tANI_U8 *pAddIEScan; //this contains the additional IE in (unicast) probe request at the time of join
    tANI_U32 nAddIEAssocLength;      //the byte count for pAddIeAssocIE
    tANI_U8 *pAddIEAssoc; //this contains the additional IE in (re) assoc request

    tANI_TIMESTAMP roamingStartTime;    //in units of 10ms
    tCsrTimerInfo roamingTimerInfo;
    eCsrRoamingReason roamingReason;
    tANI_BOOLEAN fCancelRoaming;
    vos_timer_t hTimerRoaming;
    eCsrRoamResult roamResult;  //the roamResult that is used when the roaming timer fires
    tCsrRoamJoinStatus joinFailStatusCode;    //This is the reason code for join(assoc) failure
    //The status code returned from PE for deauth or disassoc (in case of lostlink), or our own dynamic roaming
    tANI_U32 roamingStatusCode;
    tANI_U16 NumPmkidCandidate;
    tPmkidCandidateInfo PmkidCandidateInfo[CSR_MAX_PMKID_ALLOWED];
 #ifdef FEATURE_WLAN_WAPI
    tANI_U16 NumBkidCandidate;
    tBkidCandidateInfo BkidCandidateInfo[CSR_MAX_BKID_ALLOWED];
#endif
    tANI_BOOLEAN fWMMConnection;
    tANI_BOOLEAN fQOSConnection;

#ifdef FEATURE_WLAN_ESE
    tCsrEseCckmInfo eseCckmInfo;
    tANI_BOOLEAN isPrevApInfoValid;
    tSirMacSSid prevApSSID;
    tCsrBssid prevApBssid;
    tANI_U8 prevOpChannel;
    tANI_U16 clientDissSecs;
    tANI_U32 roamTS1;
#if defined(FEATURE_WLAN_ESE_UPLOAD)
    tCsrEseCckmIe suppCckmIeInfo;
#endif
#endif
    tANI_U8 bRefAssocStartCnt;   //Tracking assoc start indication
    //ht config
    tSirHTConfig        htConfig;
#ifdef FEATURE_WLAN_SCAN_PNO
    tANI_BOOLEAN pnoStarted;
#endif

#ifdef WLAN_FEATURE_ROAM_OFFLOAD
    tCsrRoamOffloadSynchStruct roamOffloadSynchParams;
    tANI_U8 psk_pmk[SIR_ROAM_SCAN_PSK_SIZE];
    size_t  pmk_len;
    tANI_U8 RoamKeyMgmtOffloadEnabled;
#endif

    /* SME FT Context */
#if defined WLAN_FEATURE_VOWIFI_11R
    tftSMEContext ftSmeContext;
#endif
    uint8_t join_bssid_count; /* This count represents the number of
                               * bssid's we are trying to join.
                               */
    tCsrRoamStoredProfile stored_roam_profile;
    bool ch_switch_in_progress;
    bool supported_nss_1x1;
    bool disable_hi_rssi;
    bool dhcp_done;
#ifdef WLAN_FEATURE_FILS_SK
    bool is_fils_connection;
    uint16_t fils_seq_num;
#endif
} tCsrRoamSession;

typedef struct tagCsrRoamStruct
{
    tANI_U32 nextRoamId;
    tDblLinkList roamCmdPendingList;
    tDblLinkList channelList5G;
    tDblLinkList channelList24;
    tCsrConfig configParam;
    tANI_U32 numChannelsEeprom; //total channels of eeprom
    tCsrChannel base20MHzChannels;   //The channel base to work on
    tCsrChannel base40MHzChannels;   //center channels for 40MHz channels
    eCsrRoamState curState[CSR_ROAM_SESSION_MAX];
    eCsrRoamSubState curSubState[CSR_ROAM_SESSION_MAX];
    eCsrRoamState prev_state[CSR_ROAM_SESSION_MAX];
    //This may or may not have the up-to-date valid channel list
    //It is used to get WNI_CFG_VALID_CHANNEL_LIST and not allocate memory all the time
    tSirMacChanNum validChannelList[WNI_CFG_VALID_CHANNEL_LIST_LEN];
    tANI_U32 numValidChannels; //total number of channels in CFG

    tANI_S32 sPendingCommands;
    vos_timer_t hTimerWaitForKey;  //To support timeout for WaitForKey state
    tCsrSummaryStatsInfo       summaryStatsInfo;
    tCsrGlobalClassAStatsInfo  classAStatsInfo;
    tCsrGlobalClassBStatsInfo  classBStatsInfo;
    tCsrGlobalClassCStatsInfo  classCStatsInfo;
    tCsrGlobalClassDStatsInfo  classDStatsInfo;
    tCsrPerStaStatsInfo        perStaStatsInfo[CSR_MAX_STA];
    struct csr_per_chain_rssi_stats_info  per_chain_rssi_stats;
    tDblLinkList  statsClientReqList;
    tDblLinkList  peStatsReqList;
    tCsrTlStatsReqInfo  tlStatsReqInfo;
    eCsrRoamLinkQualityInd vccLinkQuality;
    tCsrLinkQualityIndInfo linkQualityIndInfo;
    v_CONTEXT_t gVosContext; //used for interaction with TL
    /* TODO : Upto here */
    tCsrTimerInfo WaitForKeyTimerInfo;
    tCsrRoamSession   *roamSession;
    tANI_U32 transactionId;        // Current transaction ID for internal use.
#ifdef WLAN_FEATURE_NEIGHBOR_ROAMING
    tCsrNeighborRoamControlInfo neighborRoamInfo[CSR_ROAM_SESSION_MAX];
#endif
#ifdef FEATURE_WLAN_LFR
    tANI_U8   isFastRoamIniFeatureEnabled;
#endif
#ifdef FEATURE_WLAN_ESE
    tANI_U8   isEseIniFeatureEnabled;
#endif
#if  defined (WLAN_FEATURE_VOWIFI_11R) || defined (FEATURE_WLAN_ESE) || defined(FEATURE_WLAN_LFR)
    tANI_U8        RoamRssiDiff;
    tANI_BOOLEAN   isWESModeEnabled;
#endif
    tANI_U32 deauthRspStatus;
#ifdef WLAN_FEATURE_ROAM_OFFLOAD
    tANI_U8 *pReassocResp;  /* reassociation response from new AP */
    tANI_U16 reassocRespLen;  /* length of reassociation response */
#endif
    vos_timer_t packetdump_timer;
    tANI_BOOLEAN pending_roam_disable;
    vos_spin_lock_t roam_state_lock;
}tCsrRoamStruct;


#define GET_NEXT_ROAM_ID(pRoamStruct)  (((pRoamStruct)->nextRoamId + 1 == 0) ? 1 : (pRoamStruct)->nextRoamId)
#define CSR_IS_ROAM_STATE(pMac, state, sessionId)  ( (state) == (pMac)->roam.curState[sessionId] )

#define CSR_IS_ROAM_STOP(pMac, sessionId) CSR_IS_ROAM_STATE( (pMac), eCSR_ROAMING_STATE_STOP, sessionId )
#define CSR_IS_ROAM_INIT(pMac, sessionId)  CSR_IS_ROAM_STATE( (pMac), eCSR_ROAMING_STATE_INIT, sessionId )
#define CSR_IS_ROAM_SCANNING(pMac, sessionId)  CSR_IS_ROAM_STATE( pMac, eCSR_ROAMING_STATE_SCANNING, sessionId )
#define CSR_IS_ROAM_JOINING(pMac, sessionId)   CSR_IS_ROAM_STATE( pMac, eCSR_ROAMING_STATE_JOINING, sessionId )
#define CSR_IS_ROAM_IDLE(pMac, sessionId) CSR_IS_ROAM_STATE( pMac, eCSR_ROAMING_STATE_IDLE, sessionId )
#define CSR_IS_ROAM_JOINED(pMac, sessionId)    CSR_IS_ROAM_STATE( pMac, eCSR_ROAMING_STATE_JOINED, sessionId )

#define CSR_IS_ROAM_SUBSTATE(pMac, subState, sessionId)   ((subState) == (pMac)->roam.curSubState[sessionId])
#define CSR_IS_ROAM_SUBSTATE_JOIN_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_JOIN_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_AUTH_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_AUTH_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_REASSOC_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_REASSOC_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISASSOC_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISASSOC_NO_JOIN(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_NOTHING_TO_JOIN, sessionId)
#define CSR_IS_ROAM_SUBSTATE_REASSOC_FAIL(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_REASSOC_FAILURE, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISASSOC_FORCED(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_FORCED, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DEAUTH_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DEAUTH_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_START_BSS_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_START_BSS_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_STOP_BSS_REQ(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_STOP_BSS_REQ, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISCONNECT_CONTINUE(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISCONNECT_CONTINUE_ROAMING, sessionId)
#define CSR_IS_ROAM_SUBSTATE_CONFIG(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_CONFIG, sessionId)
#define CSR_IS_ROAM_SUBSTATE_WAITFORKEY(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_WAIT_FOR_KEY, sessionId)
#define CSR_IS_ROAM_SUBSTATE_DISASSOC_HO(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_DISASSOC_HANDOFF, sessionId)
#define CSR_IS_ROAM_SUBSTATE_HO_NT(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_JOINED_NO_TRAFFIC, sessionId)
#define CSR_IS_ROAM_SUBSTATE_HO_NRT(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_JOINED_NON_REALTIME_TRAFFIC, sessionId)
#define CSR_IS_ROAM_SUBSTATE_HO_RT(pMac, sessionId)    CSR_IS_ROAM_SUBSTATE((pMac), eCSR_ROAM_SUBSTATE_JOINED_REALTIME_TRAFFIC, sessionId)

#define CSR_IS_PHY_MODE_B_ONLY(pMac) \
        ((eCSR_DOT11_MODE_11b == (pMac)->roam.configParam.phyMode) ||\
        (eCSR_DOT11_MODE_11b_ONLY == (pMac)->roam.configParam.phyMode))

#define CSR_IS_PHY_MODE_G_ONLY(pMac) \
        (eCSR_DOT11_MODE_11g == (pMac)->roam.configParam.phyMode || eCSR_DOT11_MODE_11g_ONLY == (pMac)->roam.configParam.phyMode)

#define CSR_IS_PHY_MODE_A_ONLY(pMac) \
        (eCSR_DOT11_MODE_11a == (pMac)->roam.configParam.phyMode)

#ifdef WLAN_FEATURE_11AC
#define CSR_IS_PHY_MODE_DUAL_BAND(phyMode) \
        ((eCSR_DOT11_MODE_abg & (phyMode)) || (eCSR_DOT11_MODE_11n & (phyMode)) || \
        (eCSR_DOT11_MODE_11ac & (phyMode)) || \
        (eCSR_DOT11_MODE_AUTO & (phyMode)))
#else
#define CSR_IS_PHY_MODE_DUAL_BAND(phyMode) \
        ((eCSR_DOT11_MODE_abg & (phyMode)) || (eCSR_DOT11_MODE_11n & (phyMode)) || \
        (eCSR_DOT11_MODE_AUTO & (phyMode)))
#endif


// this function returns TRUE if the NIC is operating exclusively in the 2.4 GHz band, meaning
// it is NOT operating in the 5.0 GHz band.
#define CSR_IS_24_BAND_ONLY(pMac) \
        (eCSR_BAND_24 == (pMac)->roam.configParam.eBand)

#define CSR_IS_5G_BAND_ONLY(pMac) \
        (eCSR_BAND_5G == (pMac)->roam.configParam.eBand)

#define CSR_IS_RADIO_DUAL_BAND(pMac) \
        (eCSR_BAND_ALL == (pMac)->roam.configParam.bandCapability)

#define CSR_IS_RADIO_BG_ONLY(pMac) \
        (eCSR_BAND_24 == (pMac)->roam.configParam.bandCapability)

// this function returns TRUE if the NIC is operating exclusively in the 5.0 GHz band, meaning
// it is NOT operating in the 2.4 GHz band
#define CSR_IS_RADIO_A_ONLY(pMac) \
        (eCSR_BAND_5G == (pMac)->roam.configParam.bandCapability)

// this function returns TRUE if the NIC is operating in both bands.
#define CSR_IS_OPEARTING_DUAL_BAND(pMac) \
        ((eCSR_BAND_ALL == (pMac)->roam.configParam.bandCapability) && (eCSR_BAND_ALL == (pMac)->roam.configParam.eBand))

// this function returns TRUE if the NIC can operate in the 5.0 GHz band (could operate in the
// 2.4 GHz band also).
#define CSR_IS_OPERATING_A_BAND(pMac) \
        (CSR_IS_OPEARTING_DUAL_BAND((pMac)) || CSR_IS_RADIO_A_ONLY((pMac)) || CSR_IS_5G_BAND_ONLY((pMac)))

// this function returns TRUE if the NIC can operate in the 2.4 GHz band (could operate in the
// 5.0 GHz band also).
#define CSR_IS_OPERATING_BG_BAND(pMac) \
        (CSR_IS_OPEARTING_DUAL_BAND((pMac)) || CSR_IS_RADIO_BG_ONLY((pMac)) || CSR_IS_24_BAND_ONLY((pMac)))

#define CSR_IS_CHANNEL_5GHZ(chnNum) \
        (((chnNum) >= CSR_MIN_5GHz_CHANNEL_NUMBER) && ((chnNum) <= CSR_MAX_5GHz_CHANNEL_NUMBER))

#define CSR_IS_CHANNEL_DFS(chnNum) \
         (NV_CHANNEL_ENABLE != vos_nv_getChannelEnabledState(chnNum))

#define CSR_IS_CHANNEL_24GHZ(chnNum) \
        (((chnNum) > 0) && ((chnNum) <= CSR_MAX_24GHz_CHANNEL_NUMBER))

#define CSR_IS_SAME_BAND_CHANNELS(ch1, ch2) (CSR_IS_CHANNEL_5GHZ(ch1) == CSR_IS_CHANNEL_5GHZ(ch2))


#define CSR_IS_11D_INFO_FOUND(pMac) \
        (0 != (pMac)->scan.channelOf11dInfo)
// DEAUTHIND
#define CSR_IS_ROAMING(pSession) ((CSR_IS_LOSTLINK_ROAMING((pSession)->roamingReason)) || \
                              (eCsrDynamicRoaming == (pSession)->roamingReason)  || \
                              (eCsrReassocRoaming == (pSession)->roamingReason))


#define CSR_IS_SET_KEY_COMMAND( pCommand )    ( eSmeCommandSetKey == (pCommand)->command )

#define CSR_IS_ADDTS_WHEN_ACMOFF_SUPPORTED(pMac) (pMac->roam.configParam.addTSWhenACMIsOff)
// DEAUTHIND
#define CSR_IS_LOSTLINK_ROAMING(reason)  ((eCsrLostlinkRoamingDisassoc == (reason)) || (eCsrLostlinkRoamingDeauth == (reason)))

#define CSR_IS_ROAMING_COMMAND(pCommand) ((eCsrLostLink1 == (pCommand)->u.roamCmd.roamReason) ||\
                                          (eCsrLostLink2 == (pCommand)->u.roamCmd.roamReason) ||\
                                          (eCsrLostLink3 == (pCommand)->u.roamCmd.roamReason) )


//Stop CSR from asking for IMPS, This function doesn't disable IMPS from CSR
void csrScanSuspendIMPS( tpAniSirGlobal pMac );
//Start CSR from asking for IMPS. This function doesn't trigger CSR to request entering IMPS
//because IMPS maybe disabled.
void csrScanResumeIMPS( tpAniSirGlobal pMac );

eHalStatus csrInitGetChannels(tpAniSirGlobal pMac);
eHalStatus csrScanFilter11dResult(tpAniSirGlobal pMac);

eHalStatus csrScanFilterResults(tpAniSirGlobal pMac);

eHalStatus csrSetModifyProfileFields(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                     tCsrRoamModifyProfileFields *pModifyProfileFields);
/* ---------------------------------------------------------------------------
    \fn csrGetModifyProfileFields
    \brief HDD or SME - QOS calls this function to get the current values of
    connected profile fields changing which can cause reassoc.
    This function must be called after CFG is downloaded and STA is in connected
    state.
    \param pModifyProfileFields - pointer to the connected profile fields
    changing which can cause reassoc

    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csrGetModifyProfileFields(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                     tCsrRoamModifyProfileFields * pModifyProfileFields);
void csrSetGlobalCfgs( tpAniSirGlobal pMac );
void csrSetDefaultDot11Mode( tpAniSirGlobal pMac );
void csrScanSetChannelMask(tpAniSirGlobal pMac, tCsrChannelInfo *pChannelInfo);
tANI_BOOLEAN csrIsConnStateDisconnected(tpAniSirGlobal pMac, tANI_U32 sessionId);
tANI_BOOLEAN csrIsConnStateConnectedIbss( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateDisconnectedIbss( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateConnectedInfra( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateConnected( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateInfra( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateIbss( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateWds( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateConnectedWds( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsConnStateDisconnectedWds( tpAniSirGlobal pMac, tANI_U32 sessionId );
tANI_BOOLEAN csrIsAnySessionInConnectState( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsAllSessionDisconnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsStaSessionConnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsP2pOrSapSessionConnected(tpAniSirGlobal pMac);
tANI_BOOLEAN csrIsAnySessionConnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsInfraConnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsConcurrentInfraConnected( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsConcurrentSessionRunning( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsInfraApStarted( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsIBSSStarted( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsBTAMPStarted( tpAniSirGlobal pMac );
tANI_BOOLEAN csrIsBTAMP( tpAniSirGlobal pMac, tANI_U32 sessionId );
eHalStatus csrIsBTAMPAllowed( tpAniSirGlobal pMac, tANI_U32 chnId );
tANI_BOOLEAN csrIsValidMcConcurrentSession(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                                  tSirBssDescription *pBssDesc);
tANI_BOOLEAN csrIsConnStateConnectedInfraAp( tpAniSirGlobal pMac, tANI_U32 sessionId );
bool csr_is_ndi_started(tpAniSirGlobal mac_ctx, uint32_t session_id);
/*----------------------------------------------------------------------------
  \fn csrRoamRegisterLinkQualityIndCallback

  \brief
  a CSR function to allow HDD to register a callback handler with CSR for
  link quality indications.

  Only one callback may be registered at any time.
  In order to deregister the callback, a NULL cback may be provided.

  Registration happens in the task context of the caller.

  \param callback - Call back being registered
  \param pContext - user data

  DEPENDENCIES: After CSR open

  \return eHalStatus
-----------------------------------------------------------------------------*/
eHalStatus csrRoamRegisterLinkQualityIndCallback(tpAniSirGlobal pMac,
                                                 csrRoamLinkQualityIndCallback   callback,
                                                 void                           *pContext);
/* ---------------------------------------------------------------------------
    \fn csrGetStatistics
    \brief csr function that client calls to register a callback to get
    different PHY level statistics from CSR.

    \param requesterId - different client requesting for statistics, HDD, UMA/GAN etc
    \param statsMask - The different category/categories of stats requester is looking for
    \param callback - SME sends back the requested stats using the callback
    \param periodicity - If requester needs periodic update, 0 means it's an one
                         time request
    \param cache - If requester is happy with cached stats
    \param staId - The station ID for which the stats is requested for
    \param pContext - user context to be passed back along with the callback
    \param sessionId - sme session Id.
    \return eHalStatus
  ---------------------------------------------------------------------------*/
eHalStatus csrGetStatistics(tpAniSirGlobal pMac, eCsrStatsRequesterType requesterId,
                            tANI_U32 statsMask,
                            tCsrStatsCallback callback,
                            tANI_U32 periodicity, tANI_BOOLEAN cache,
                            tANI_U8 staId, void *pContext,
                            tANI_U8 sessionId);

/* ---------------------------------------------------------------------------
    \fn csrGetTLSTAState
    \helper function to get the TL STA State whenever the function is called.

    \param staId - The staID to be passed to the TL
            to get the relevant TL STA State
    \return the state as tANI_U16
  ---------------------------------------------------------------------------*/
tANI_U16 csrGetTLSTAState(tpAniSirGlobal pMac, tANI_U8 staId);

/* ---------------------------------------------------------------------------
    \fn csrGetRssi
    \ creates SME req packet for getRSSI and post to Self

    \param pMac     - global MAC context
    \param callback - hdd callback function to be called once FW returns the
                      RSSI value
    \param staId    - The staID to be passed to the TL to get the relevant
                      TL STA State
    \param bssID    - bssid for which RSSI is requested
    \param lastRSSI - RSSI value at time of request. In case request cannot
                      be sent to firmware, do not hold up but return this value.
    \param pContext - user context to be passed back along with the callback
    \param pVosContext - vos conext
    \return the state as tANI_U16
  ---------------------------------------------------------------------------*/
eHalStatus csrGetRssi(tpAniSirGlobal pMac,tCsrRssiCallback callback,
                      tANI_U8 staId, tCsrBssid bssId, tANI_S8 lastRSSI,
                      void * pContext,void * pVosContext);

/* ---------------------------------------------------------------------------
    \fn csrGetSnr
    \brief csr function that client calls to register a callback to get
     SNR stored in TL

    \param callback - SME sends back the requested stats using the callback
    \param staId - The station ID for which the stats is requested for
    \param bssid - The bssid for the connected session
    \param pContext - user context to be passed back along with the callback

    \return eHalStatus
  ---------------------------------------------------------------------------*/
eHalStatus csrGetSnr(tpAniSirGlobal pMac, tCsrSnrCallback callback,
                     tANI_U8 staId, tCsrBssid bssId, void *pContext);

#if defined(FEATURE_WLAN_ESE) && defined(FEATURE_WLAN_ESE_UPLOAD)
eHalStatus csrGetTsmStats(tpAniSirGlobal pMac,
                          tCsrTsmStatsCallback callback,
                          tANI_U8 staId,
                          tCsrBssid bssId,
                          void *pContext,
                          void* pVosContext,
                          tANI_U8 tid);
#endif  /* FEATURE_WLAN_ESE && FEATURE_WLAN_ESE_UPLOAD */

/* ---------------------------------------------------------------------------
    \fn csrGetConfigParam
    \brief HDD calls this function to get the global settings currently maintained by CSR.
    \param pParam - caller allocated memory
    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csrGetConfigParam(tpAniSirGlobal pMac, tCsrConfigParam *pParam);

/* ---------------------------------------------------------------------------
    \fn csrMsgProcessor
    \brief HDD calls this function to change some global settings.
    caller must set the all fields or call csrGetConfigParam to prefill the fields.
    \param pParam - caller allocated memory
    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csrChangeDefaultConfigParam(tpAniSirGlobal pMac, tCsrConfigParam *pParam);


/* ---------------------------------------------------------------------------
    \fn csrMsgProcessor
    \brief HDD calls this function for the messages that are handled by CSR.
    \param pMsgBuf - a pointer to a buffer that maps to various structures base on the message type.
    The beginning of the buffer can always map to tSirSmeRsp.
    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csrMsgProcessor( tpAniSirGlobal pMac,  void *pMsgBuf );

/* ---------------------------------------------------------------------------
    \fn csrOpen
    \brief This function must be called before any API call to CSR.
    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csrOpen(tpAniSirGlobal pMac);
/* ---------------------------------------------------------------------------
    \fn csr_init_chan_list
    \brief This function needs to called to initialize channel information,
     which, for discrete, will come from targer fw.
    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csr_init_chan_list(tpAniSirGlobal mac, v_U8_t *alpha2);

/* ---------------------------------------------------------------------------
    \fn csrClose
    \brief To close down CSR module. There should not be any API call into CSR after calling this function.
    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csrClose(tpAniSirGlobal pMac);
/* ---------------------------------------------------------------------------
    \fn csrStart
    \brief To start CSR.
    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csrStart(tpAniSirGlobal pMac);
/* ---------------------------------------------------------------------------
    \fn csrStop
    \brief To stop CSR. CSR still keeps its current setting.
    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csrStop(tpAniSirGlobal pMac, tHalStopType stopType);
/* ---------------------------------------------------------------------------
    \fn csrReady
    \brief To let CSR is ready to operate
    \return eHalStatus
  -------------------------------------------------------------------------------*/
eHalStatus csrReady(tpAniSirGlobal pMac);

#ifdef FEATURE_WLAN_WAPI

eHalStatus csrScanGetBKIDCandidateList(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                       tBkidCandidateInfo *pBkidList, tANI_U32 *pNumItems );
/* ---------------------------------------------------------------------------
    \fn csrRoamGetWapiReqIE
    \brief return the WAPI IE CSR passes to PE to JOIN request or START_BSS request
    \param pLen - caller allocated memory that has the length of pBuf as input. Upon returned, *pLen has the
    needed or IE length in pBuf.
    \param pBuf - Caller allocated memory that contain the IE field, if any, upon return
    \return eHalStatus - when fail, it usually means the buffer allocated is not big enough
  -------------------------------------------------------------------------------*/
eHalStatus csrRoamGetWapiReqIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf);

/* ---------------------------------------------------------------------------
    \fn csrRoamGetWapiRspIE
    \brief return the WAPI IE from the beacon or probe rsp if connected
    \param pLen - caller allocated memory that has the length of pBuf as input. Upon returned, *pLen has the
    needed or IE length in pBuf.
    \param pBuf - Caller allocated memory that contain the IE field, if any, upon return
    \return eHalStatus - when fail, it usually means the buffer allocated is not big enough
  -------------------------------------------------------------------------------*/
eHalStatus csrRoamGetWapiRspIE(tpAniSirGlobal pMac, tANI_U32 sessionId, tANI_U32 *pLen, tANI_U8 *pBuf);
tANI_U8 csrConstructWapiIe( tpAniSirGlobal pMac, tANI_U32 sessionId, tCsrRoamProfile *pProfile,
                            tSirBssDescription *pSirBssDesc, tDot11fBeaconIEs *pIes, tCsrWapiIe *pWapiIe );
#endif /* FEATURE_WLAN_WAPI */

eHalStatus csrRoamUpdateAPWPSIE( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirAPWPSIEs *pAPWPSIES );
eHalStatus csrRoamUpdateWPARSNIEs( tpAniSirGlobal pMac, tANI_U32 sessionId, tSirRSNie * pAPSirRSNie);
void csrSetCfgPrivacy( tpAniSirGlobal pMac, tCsrRoamProfile *pProfile, tANI_BOOLEAN fPrivacy );
tANI_S8 csrGetInfraSessionId( tpAniSirGlobal pMac );
tANI_U8 csrGetInfraOperationChannel( tpAniSirGlobal pMac, tANI_U8 sessionId);
tANI_BOOLEAN csrIsSessionClientAndConnected(tpAniSirGlobal pMac, tANI_U8 sessionId);
tANI_U8 csrGetConcurrentOperationChannel( tpAniSirGlobal pMac );

#ifdef FEATURE_WLAN_MCC_TO_SCC_SWITCH
v_U16_t csrCheckConcurrentChannelOverlap(tpAniSirGlobal pMac, v_U16_t sap_ch,
                               eCsrPhyMode sap_phymode, v_U8_t cc_switch_mode );
#endif
eHalStatus csrRoamCopyConnectProfile(tpAniSirGlobal pMac, tANI_U32 sessionId,
                               tCsrRoamConnectedProfile *pProfile);
tANI_BOOLEAN csrIsSetKeyAllowed(tpAniSirGlobal pMac, tANI_U32 sessionId);

void csrSetOppositeBandChannelInfo( tpAniSirGlobal pMac );
void csrConstructCurrentValidChannelList( tpAniSirGlobal pMac, tDblLinkList *pChannelSetList,
                                            tANI_U8 *pChannelList, tANI_U8 bSize, tANI_U8 *pNumChannels );
#ifdef FEATURE_WLAN_SCAN_PNO
eHalStatus csrScanSavePreferredNetworkFound(tpAniSirGlobal pMac,
            tSirPrefNetworkFoundInd *pPrefNetworkFoundInd);
#endif

#ifdef WLAN_FEATURE_VOWIFI_11R
//Returns whether the current association is a 11r assoc or not
tANI_BOOLEAN csrRoamIs11rAssoc(tpAniSirGlobal pMac, tANI_U8 sessionId);
#endif

#ifdef FEATURE_WLAN_ESE
//Returns whether the current association is a ESE assoc or not
tANI_BOOLEAN csrRoamIsESEAssoc(tpAniSirGlobal pMac, tANI_U8 sessionId);
tANI_BOOLEAN csrRoamIsEseIniFeatureEnabled(tpAniSirGlobal pMac);
tANI_BOOLEAN csrNeighborRoamIsESEAssoc(tpAniSirGlobal pMac, tANI_U32 sessionId);
#endif

//Remove this code once SLM_Sessionization is supported
//BMPS_WORKAROUND_NOT_NEEDED
void csrDisconnectAllActiveSessions(tpAniSirGlobal pMac);

#ifdef FEATURE_WLAN_LFR
//Returns whether "Legacy Fast Roaming" is enabled...or not
tANI_BOOLEAN csrRoamIsFastRoamEnabled(tpAniSirGlobal pMac, tANI_U32 sessionId);
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
tANI_BOOLEAN csrRoamIsRoamOffloadScanEnabled(tpAniSirGlobal pMac);
#endif
tANI_BOOLEAN csrIsChannelPresentInList( tANI_U8 *pChannelList, int  numChannels, tANI_U8   channel );
VOS_STATUS csrAddToChannelListFront( tANI_U8 *pChannelList, int  numChannels, tANI_U8   channel );
#ifdef WLAN_FEATURE_ROAM_SCAN_OFFLOAD
eHalStatus csrScanRequestLfrResult(tpAniSirGlobal pMac, tANI_U32 sessionId,
                                   csrScanCompleteCallback callback, void *pContext);
eHalStatus csrRoamOffloadScanRspHdlr(tpAniSirGlobal pMac,
                                     tpSirRoamOffloadScanRsp scanOffloadRsp);
eHalStatus csrHandoffRequest(tpAniSirGlobal pMac, tANI_U8 sessionId,
                             tCsrHandoffRequest *pHandoffInfo);
#endif
tANI_BOOLEAN csrRoamIsStaMode(tpAniSirGlobal pMac, tANI_U32 sessionId);
#endif

/* Post Channel Change Indication */
eHalStatus csrRoamChannelChangeReq(tpAniSirGlobal pMac, tCsrBssid bssid,
                        tANI_U8 cbMode, tCsrRoamProfile *pprofile);

/* Post Beacon Tx Start Indication */
eHalStatus csrRoamStartBeaconReq( tpAniSirGlobal pMac,
                     tCsrBssid bssid, tANI_U8 dfsCacWaitStatus);

eHalStatus
csrRoamSendChanSwIERequest(tpAniSirGlobal pMac, tCsrBssid bssid,
                     tANI_U8 targetChannel, tANI_U8 csaIeReqd,
                     u_int8_t ch_bandwidth);

/*----------------------------------------------------------------------------
 \fn csrRoamModifyAddIEs
 \brief  This function sends msg to modify the additional IE buffers in PE
 \param  pMac - pMac global structure
 \param  pModifyIE - pointer to tSirModifyIE structure
 \param  updateType - Type of buffer
 \- return Success or failure
-----------------------------------------------------------------------------*/
eHalStatus
csrRoamModifyAddIEs(tpAniSirGlobal pMac,
                             tSirModifyIE *pModifyIE,
                             eUpdateIEsType updateType);


/*----------------------------------------------------------------------------
 \fn csrRoamUpdateAddIEs
 \brief  This function sends msg to updates the additional IE buffers in PE
 \param  pMac - pMac global structure
 \param  pUpdateIE - pointer to tSirUpdateIE structure
 \param  updateType, - type of buffer
 \- return Success or failure
-----------------------------------------------------------------------------*/
eHalStatus
csrRoamUpdateAddIEs(tpAniSirGlobal pMac,
                  tSirUpdateIE *pUpdateIE,
                  eUpdateIEsType updateType);

#ifdef WLAN_FEATURE_ROAM_OFFLOAD
void csrProcessRoamOffloadSynchInd(tHalHandle hHal,
            tpSirRoamOffloadSynchInd pSmeRoamOffloadSynchInd);
eHalStatus csrScanSaveRoamOffloadApToScanCache(tpAniSirGlobal pMac,
            tSirRoamOffloadSynchInd *pRoamOffloadSynchInd);
void csrProcessHOFailInd(tpAniSirGlobal pMac, void *pMsgBuf);
#endif
bool csr_store_joinreq_param(tpAniSirGlobal mac_ctx,
                             tCsrRoamProfile *profile,
                             tScanResultHandle scan_cache,
                             uint32_t *roam_id,
                             uint32_t session_id);
bool csr_clear_joinreq_param(tpAniSirGlobal mac_ctx,
                             tANI_U32 session_id);
eHalStatus csr_issue_stored_joinreq(tpAniSirGlobal mac_ctx,
                                    uint32_t *roam_id,
                                    uint32_t session_id);
#ifdef FEATURE_WLAN_DIAG_SUPPORT
void csr_diag_event_report(tpAniSirGlobal pmac, uint16_t event_type,
                           uint16_t status, uint16_t reasoncode);
#endif
#endif

