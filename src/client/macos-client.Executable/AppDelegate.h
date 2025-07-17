#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

- (void) resize:(NSRect *)rect;
- (void) draw;

- (void) keyDown:(NSEvent *)event;
- (void) keyUp:(NSEvent *)event;

- (void) mouseDown:(NSEvent *)event;
- (void) mouseMoved:(NSEvent *)event;
- (void) mouseDragged:(NSEvent *)event;
- (void) mouseUp:(NSEvent *)event;
- (void) scrollWheel:(NSEvent *)event;

@end

#if !defined _IMPORT
#define Application ((AppDelegate *)[NSApplication sharedApplication].delegate)
#endif

