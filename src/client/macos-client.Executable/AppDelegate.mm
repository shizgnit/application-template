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
    if(instance == nil) {
        assets->init();
        assets->set("shader.path", std::string("shaders-gl"));
        assets->set("shader.version", std::string("#version 410 core"));
        
        instance = new app();
        instance->dimensions(rect->size.width / 2, rect->size.height / 2);
        instance->on_startup();
        instance->started = true;
    }
    else {
        instance->dimensions(rect->size.width, rect->size.height);
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
    gui->raise({ platform::input::KEY, platform::input::DOWN, key, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
    input->raise({ platform::input::KEY, platform::input::DOWN, key, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
}
- (void) keyUp:(NSEvent *)event {
    //unichar hid = [[event charactersIgnoringModifiers] characterAtIndex:0];
    int keyCode = event.keyCode;
    int key = platform::keys[keyCode].vkey;
    gui->raise({ platform::input::KEY, platform::input::UP, key, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
    input->raise({ platform::input::KEY, platform::input::UP, key, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
}
- (void) mouseDown:(NSEvent *)event {
    NSPoint global = [NSEvent mouseLocation];
    NSPoint point = [event locationInWindow];
    point.y = instance->getHeight() - point.y;
    
    NSUInteger button = [NSEvent pressedMouseButtons];
    
    gui->raise({ platform::input::POINTER, platform::input::DOWN, 1, 0, 0.0f, { point.x, point.y, 0.0f } }, point.x, point.y);
    input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 0, 0.0f, { point.x, point.y, 0.0f } });
}
- (void) mouseMoved:(NSEvent *)event {
    NSPoint point = [event locationInWindow];
    point.y = instance->getHeight() - point.y;
    
    gui->raise({ platform::input::POINTER, platform::input::MOVE, 0, 0, 0.0f, { point.x, point.y, 0.0f } }, point.x, point.y);
    input->raise({ platform::input::POINTER, platform::input::MOVE, 0, 1, 0.0f, { point.x, point.y, 0.0f } });
}
- (void) mouseDragged:(NSEvent *)event {
    NSPoint point = [event locationInWindow];
    point.y = instance->getHeight() - point.y;
    
    gui->raise({ platform::input::POINTER, platform::input::MOVE, 0, 0, 0.0f, { point.x, point.y, 0.0f } }, point.x, point.y);
    input->raise({ platform::input::POINTER, platform::input::MOVE, 0, 1, 0.0f, { point.x, point.y, 0.0f } });
}
- (void) mouseUp:(NSEvent *)event {
    NSPoint point = [event locationInWindow];
    point.y = instance->getHeight() - point.y;
    
    gui->raise({ platform::input::POINTER, platform::input::UP, 1, 0, 0.0f, { point.x, point.y, 0.0f } }, point.x, point.y);
    input->raise({ platform::input::POINTER, platform::input::UP, 1, 0, 0.0f, { point.x, point.y, 0.0f } });

}

@end
