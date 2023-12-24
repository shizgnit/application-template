//
//  AppDelegate.h
//  ios-client.Application
//
//  Created by Dee Abbott on 7/31/22.
//

#import <UIKit/UIKit.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>

- (void) resize:(CGRect *)rect;
- (void) draw;

- (void) keyDown:(UIPressesEvent *)event;
- (void) keyUp:(UIPressesEvent *)event;

- (void) mouseDown:(UIEvent *)event;
- (void) mouseDragged:(UIEvent *)event;
- (void) mouseUp:(UIEvent *)event;

@end

#if !defined _IMPORT
#define Application ((AppDelegate *)[UIApplication sharedApplication].delegate)
#endif

