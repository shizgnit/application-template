//
//  AppDelegate.m
//  macos-client.Executable
//
//  Created by Dee Abbott on 7/31/22.
//

#define _IMPORT
#import "AppDelegate.h"
#undef _IMPORT

#import "engine.hpp"
#import "application.hpp"


@interface AppDelegate ()
{
    application *instance;
}

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


- (BOOL)applicationSupportsSecureRestorableState:(NSApplication *)app {
    return YES;
}

- (void) resize:(NSRect *)rect {
    NSScreen * main = [NSScreen mainScreen];
    CGFloat scale = main.backingScaleFactor;
    if(instance == nil) {
        assets->init();
        assets->set("shader.path", std::string("shaders-apple"));
        assets->set("shader.version", std::string("#version 410 core"));
        
        instance = new app();
        instance->dimensions(rect->size.width, rect->size.height, scale);
        instance->on_startup();
        instance->started = true;
    }
    else {
        instance->dimensions(rect->size.width, rect->size.height, scale);
    }
}
- (void) draw {
    instance->on_interval();
    instance->on_draw();
}

- (void) keyDown:(NSEvent *)event {
    //unichar hid = [[event charactersIgnoringModifiers] characterAtIndex:0];
    int keyCode = event.keyCode;
    int key = platform::keys[keyCode].vkey;
    if(gui->raise({ platform::input::KEY, platform::input::DOWN, key, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0) == false) {
        input->raise({ platform::input::KEY, platform::input::DOWN, key, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
    }
}
- (void) keyUp:(NSEvent *)event {
    //unichar hid = [[event charactersIgnoringModifiers] characterAtIndex:0];
    int keyCode = event.keyCode;
    int key = platform::keys[keyCode].vkey;
    if(gui->raise({ platform::input::KEY, platform::input::UP, key, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0) == false) {
        input->raise({ platform::input::KEY, platform::input::UP, key, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
    }
}
- (void) mouseDown:(NSEvent *)event {
    NSPoint global = [NSEvent mouseLocation];
    NSPoint point = [event locationInWindow];
    point.y = (instance->getHeight() / instance->getScale()) - point.y;
   
    point.x = point.x * instance->getScale();
    point.y = point.y * instance->getScale();
    
    NSUInteger button = [NSEvent pressedMouseButtons];
    
    if(gui->raise({ platform::input::POINTER, platform::input::DOWN, 1, 0, 0.0f, { point.x, point.y, 0.0f } }, point.x, point.y) == false) {
        input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 0, 0.0f, { point.x, point.y, 0.0f } });
    }
}
- (void) mouseMoved:(NSEvent *)event {
    NSPoint point = [event locationInWindow];
    point.y = (instance->getHeight() / instance->getScale()) - point.y;

    point.x = point.x * instance->getScale();
    point.y = point.y * instance->getScale();

    if(gui->raise({ platform::input::POINTER, platform::input::MOVE, 0, 0, 0.0f, { point.x, point.y, 0.0f } }, point.x, point.y) == false) {
        input->raise({ platform::input::POINTER, platform::input::MOVE, 0, 1, 0.0f, { point.x, point.y, 0.0f } });
    }
}
- (void) mouseDragged:(NSEvent *)event {
    NSPoint point = [event locationInWindow];
    point.y = (instance->getHeight() / instance->getScale()) - point.y;
   
    point.x = point.x * instance->getScale();
    point.y = point.y * instance->getScale();
   
    if(gui->raise({ platform::input::POINTER, platform::input::MOVE, 0, 0, 0.0f, { point.x, point.y, 0.0f } }, point.x, point.y) == false) {
        input->raise({ platform::input::POINTER, platform::input::MOVE, 0, 1, 0.0f, { point.x, point.y, 0.0f } });
    }
}
- (void) mouseUp:(NSEvent *)event {
    NSPoint point = [event locationInWindow];
    point.y = (instance->getHeight() / instance->getScale()) - point.y;
    
    point.x = point.x * instance->getScale();
    point.y = point.y * instance->getScale();
     
    if(gui->raise({ platform::input::POINTER, platform::input::UP, 1, 0, 0.0f, { point.x, point.y, 0.0f } }, point.x, point.y) == false) {
        input->raise({ platform::input::POINTER, platform::input::UP, 1, 0, 0.0f, { point.x, point.y, 0.0f } });
    }
}
- (void) scrollWheel:(NSEvent *)event {
    NSPoint point = [event locationInWindow];
    CGFloat travel = [event scrollingDeltaY];
    
    point.x = point.x * instance->getScale();
    point.y = point.y * instance->getScale();
   
    if(gui->raise({ platform::input::POINTER, platform::input::WHEEL, 0, 0, (float)travel, { point.x, point.y, 0.0f } }, point.x, point.y) == false) {
        input->raise({ platform::input::POINTER, platform::input::WHEEL, 0, 0, (float)travel, { point.x, point.y, 0.0f } });
    }
}

@end
