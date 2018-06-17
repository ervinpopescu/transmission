// This file Copyright © Transmission authors and contributors.
// It may be used under the MIT (SPDX: MIT) license.
// License text can be found in the licenses/ folder.

#import <AppKit/AppKit.h>

#import "InfoViewController.h"

@interface InfoOptionsViewController : NSViewController <InfoViewController>
{
    NSArray * fTorrents;
    
    BOOL fSet;
    
    IBOutlet NSPopUpButton * fPriorityPopUp, * fRatioPopUp, * fIdlePopUp;
    IBOutlet NSButton * fUploadLimitCheck, * fDownloadLimitCheck, * fGlobalLimitCheck, * fRemoveSeedingCompleteCheck, * fSequentialCheck;
    IBOutlet NSTextField * fUploadLimitField, * fDownloadLimitField, * fRatioLimitField, * fIdleLimitField,
                        * fUploadLimitLabel, * fDownloadLimitLabel, * fIdleLimitLabel,
                        * fRatioLimitGlobalLabel, * fIdleLimitGlobalLabel,
                        * fPeersConnectLabel, * fPeersConnectField;
    
    //remove when we switch to auto layout on 10.7
    IBOutlet NSTextField * fTransferBandwidthSectionLabel, * fPrioritySectionLabel, * fPriorityLabel;
    IBOutlet NSTextField * fSeedingLimitsSectionLabel, * fRatioLabel, * fInactivityLabel;
    IBOutlet NSTextField * fAdvancedSectionLabel, * fMaxConnectionsLabel;
    
    NSString * fInitialString;
}

- (NSRect)viewRect;
- (void)checkLayout;
- (void)checkWindowSize;
- (void)updateWindowLayout;

- (void)setInfoForTorrents:(NSArray<Torrent*>*)torrents;
- (void)updateInfo;
- (void)updateOptions;

- (IBAction)setUseSpeedLimit:(id)sender;
- (IBAction)setSpeedLimit:(id)sender;
- (IBAction)setUseGlobalSpeedLimit:(id)sender;

- (IBAction)setRatioSetting:(id)sender;
- (IBAction)setRatioLimit:(id)sender;

- (IBAction)setIdleSetting:(id)sender;
- (IBAction)setIdleLimit:(id)sender;

- (IBAction)setRemoveWhenSeedingCompletes:(id)sender;

- (IBAction)setPriority:(id)sender;

- (IBAction)setPeersConnectLimit:(id)sender;

@property(nonatomic) IBOutlet NSView* fPriorityView;
@property(nonatomic) CGFloat oldHeight;

- (IBAction) setSequential: (id) sender;

@end
