// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#import "IECore/IERenderer.h"

#if defined (__APPLE__)

#import <Cocoa/Cocoa.h>

@interface IEAppleStatusItem : NSObject<NSApplicationDelegate>
@property IERenderer* Renderer;
@property (strong, nonatomic) NSStatusItem* StatusItem;
@end

@implementation IEAppleStatusItem
- (id)initWithRenderer:(IERenderer*)Renderer
{
    self = [super init];
    if (self) 
    {
        self.Renderer = Renderer;
        [self createStatusBarIcon];
    }    
    return self;
}

- (void)createStatusBarIcon
{
    NSStatusBar* StatusBar = [NSStatusBar systemStatusBar];
    self.StatusItem = [StatusBar statusItemWithLength:NSSquareStatusItemLength];
    if (self.StatusItem && self.Renderer)
    {
        NSString* LogoFilePath = [NSString stringWithUTF8String:self.Renderer->GetIEIconPathString().c_str()];
        NSImage* Icon = [[NSImage alloc] initWithContentsOfFile:LogoFilePath];
        if (Icon)
        {
            [Icon setSize:NSMakeSize([StatusBar thickness] - 5, [StatusBar thickness] - 5)];
            [self.StatusItem button].image = Icon;
                
            NSMenu* MenuItem = [[NSMenu alloc] initWithTitle:@"IEMidi"];
            [MenuItem setMinimumWidth:200];
            [MenuItem setAutoenablesItems:NO];

            NSView *menuTitleView = [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 200, 30)];
            NSTextField *menuTitleLabel = [[NSTextField alloc] initWithFrame:NSMakeRect(10, 0, 190, 20)];
            NSFont *titleFont = [NSFont systemFontOfSize:13 weight:NSFontWeightSemibold];
            NSDictionary *titleAttributes = @{NSFontAttributeName: titleFont};
            NSAttributedString *titleAttribute = [[NSAttributedString alloc] initWithString:@"IEMidi" attributes:titleAttributes];
            [menuTitleLabel setAttributedStringValue:titleAttribute];
            [menuTitleLabel setEditable:NO];
            [menuTitleLabel setBezeled:NO];
            [menuTitleLabel setDrawsBackground:NO];
            [menuTitleView addSubview:menuTitleLabel];
            
            NSMenuItem* titleMenuItem = [[NSMenuItem alloc] initWithTitle:@"IEMidi" action:nil keyEquivalent:@""];
            [titleMenuItem setView:menuTitleView];
            [MenuItem addItem:titleMenuItem];

            NSMenuItem *subTitleMenuItem = [[NSMenuItem alloc] init];
            [subTitleMenuItem setEnabled:NO];
            NSFont *subTitleFont = [NSFont menuFontOfSize:12];
            NSDictionary *subTitleAttributes = @{NSFontAttributeName: subTitleFont};
            NSAttributedString *subTitleAttribute = [[NSAttributedString alloc] initWithString:@"App State: Running" attributes:subTitleAttributes];
            subTitleMenuItem.attributedTitle = subTitleAttribute;
            [MenuItem addItem:subTitleMenuItem];

            [MenuItem addItem:[NSMenuItem separatorItem]];
            
            NSMenuItem* editMenuItem = [[NSMenuItem alloc] initWithTitle:@"Edit" action:@selector(edit:) keyEquivalent:@""];
            [editMenuItem setTarget:self];
            [MenuItem addItem:editMenuItem];

            NSMenuItem* quitMenuItem = [[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(quit:) keyEquivalent:@""];
            [quitMenuItem setTarget:self];
            [MenuItem addItem:quitMenuItem];

            [self.StatusItem setMenu:MenuItem];
        }
    }
}

- (void)edit:(id)sender
{
    if (self.Renderer)
    {
        self.Renderer->RestoreAppWindow();
    }
}

- (void)quit:(id)sender
{
    if (self.Renderer)
    {
        self.Renderer->RequestExit();
    }
}
@end

extern "C" void InitializeIEAppleApp(IERenderer* Renderer)
{
    IEAppleStatusItem* IEStatusItem = [[IEAppleStatusItem alloc] initWithRenderer:Renderer];
}
#endif