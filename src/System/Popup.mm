//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/System/Popup.hpp"
#if NP_ENGINE_PLATFORM_IS_APPLE
#import <Cocoa/Cocoa.h>

namespace np::system
{
    Popup::Select Popup::Show(str title, str message, Popup::Style style, Popup::Buttons buttons)
    {
        NSAlert *alert = [[NSAlert alloc] init];
        
        NSAlertStyle nsstyle = NSAlertStyleInformational;
        switch (style) {
            case Popup::Style::Info:
                nsstyle = NSAlertStyleInformational;
                break;
            case Popup::Style::Warning:
                nsstyle = NSAlertStyleWarning;
                break;
            case Popup::Style::Error:
                nsstyle = NSAlertStyleCritical;
                break;
            case Popup::Style::Question:
                nsstyle = NSAlertStyleWarning;
                break;
            default:
                NP_ASSERT(false, "Popup was given an invalid style.");
                break;
        }
        
        [alert setAlertStyle: nsstyle];
        
        //TODO: set our icon like below:
        /*
         [alert setIcon:(NSImage * _Nullable)]; // NSImage -- (instancetype)initWithContentsOfFile:(NSString *)fileName;
         */
        
        [alert setMessageText: [NSString stringWithCString: title.c_str()
                                                  encoding: [NSString defaultCStringEncoding]]];
        
        [alert setInformativeText: [NSString stringWithCString: message.c_str()
                                                      encoding: [NSString defaultCStringEncoding]]];
        
        NSString* const kOkStr = @"OK";
        NSString* const kCancelStr = @"Cancel";
        NSString* const kYesStr = @"Yes";
        NSString* const kNoStr = @"No";
        NSString* const kQuitStr = @"Quit";
        
        switch (buttons) {
            case Popup::Buttons::OK:
                [alert addButtonWithTitle:kOkStr];
                break;
            case Popup::Buttons::OKCancel:
                [alert addButtonWithTitle:kOkStr];
                [alert addButtonWithTitle:kCancelStr];
                break;
            case Popup::Buttons::YesNo:
                [alert addButtonWithTitle:kYesStr];
                [alert addButtonWithTitle:kNoStr];
                break;
            case Popup::Buttons::YesNoCancel:
                [alert addButtonWithTitle:kYesStr];
                [alert addButtonWithTitle:kNoStr];
                [alert addButtonWithTitle:kCancelStr];
                break;
            case Popup::Buttons::Quit:
                [alert addButtonWithTitle:kQuitStr];
                break;
            default:
                NP_ASSERT(false, "Popup was given invalid buttons.");
                break;
        }
        
        [[alert window] setLevel:NSModalPanelWindowLevel];
        i32 index = [alert runModal];
        
        Popup::Select selection = Popup::Select::None;
        switch (buttons) {
            case Popup::Buttons::OK:
                if (index == NSAlertFirstButtonReturn)
                {
                    selection = Popup::Select::OK;
                }
                break;
            case Popup::Buttons::OKCancel:
                if (index == NSAlertFirstButtonReturn)
                {
                    selection = Popup::Select::OK;
                }
                else if (index == NSAlertSecondButtonReturn)
                {
                    selection = Popup::Select::Cancel;
                }
                break;
            case Popup::Buttons::YesNo:
                if (index == NSAlertFirstButtonReturn)
                {
                    selection = Popup::Select::Yes;
                }
                else if (index == NSAlertSecondButtonReturn)
                {
                    selection = Popup::Select::No;
                }
                break;
            case Popup::Buttons::YesNoCancel:
                if (index == NSAlertFirstButtonReturn)
                {
                    selection = Popup::Select::Yes;
                }
                else if (index == NSAlertSecondButtonReturn)
                {
                    selection = Popup::Select::No;
                }
                else if (index == NSAlertThirdButtonReturn)
                {
                    selection = Popup::Select::Cancel;
                }
                break;
            case Popup::Buttons::Quit:
                if (index == NSAlertFirstButtonReturn)
                {
                    selection = Popup::Select::Quit;
                }
                break;
        }
        
        [alert release];
        return selection;
    }
}
#else
#error This file should only be used on Apple machines
#endif
