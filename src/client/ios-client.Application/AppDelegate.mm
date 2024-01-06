//
//  AppDelegate.m
//  ios-client.Application
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


- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    return YES;
}


#pragma mark - UISceneSession lifecycle


- (UISceneConfiguration *)application:(UIApplication *)application configurationForConnectingSceneSession:(UISceneSession *)connectingSceneSession options:(UISceneConnectionOptions *)options {
    // Called when a new scene session is being created.
    // Use this method to select a configuration to create the new scene with.
    return [[UISceneConfiguration alloc] initWithName:@"Default Configuration" sessionRole:connectingSceneSession.role];
}


- (void)application:(UIApplication *)application didDiscardSceneSessions:(NSSet<UISceneSession *> *)sceneSessions {
    // Called when the user discards a scene session.
    // If any sessions were discarded while the application was not running, this will be called shortly after application:didFinishLaunchingWithOptions.
    // Use this method to release any resources that were specific to the discarded scenes, as they will not return.
}

- (void) resize:(CGRect *)rect {
    if(instance == nil) {
        assets->init();
        assets->set("shader.path", std::string("shaders-apple"));
        assets->set("shader.version", std::string("#version 300 es"));
        
        instance = new app();
        instance->dimensions(rect->size.width, rect->size.height);
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

- (void) keyDown:(UIPressesEvent *)event {
    for (UIPress* press in event.allPresses) {
        int key = platform::keys[press.key.keyCode].xref;
        NSLog(@"key: 0x%02x, 0x%02x", key, press.key.keyCode);
        gui->raise({ platform::input::KEY, platform::input::DOWN, key, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
        input->raise({ platform::input::KEY, platform::input::DOWN, key, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
    }
}
- (void) keyUp:(UIPressesEvent *)event {
    for (UIPress* press in event.allPresses) {
        int key = platform::keys[press.key.keyCode].xref;
        gui->raise({ platform::input::KEY, platform::input::UP, key, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
        input->raise({ platform::input::KEY, platform::input::UP, key, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
    }
}

- (void) mouseDown:(UIEvent *)event {
    UITouch *touch = [[event allTouches] anyObject];
    CGPoint point = [touch locationInView:touch.view];

    //if ([[touch.view class] isSubclassOfClass:[UILabel class]]) {
        //dragging = YES;
        //oldX = touchLocation.x;
        //oldY = touchLocation.y;
    //}
    
    gui->raise({ platform::input::POINTER, platform::input::DOWN, 1, 0, 0.0f, { point.x, point.y, 0.0f } }, 0, 0);
    input->raise({ platform::input::POINTER, platform::input::DOWN, 1, 1, 0.0f, { point.x, point.y, 0.0f } });
}
- (void) mouseDragged:(UIEvent *)event {
    //if ([[touch.view class] isSubclassOfClass:[UILabel class]]) {
        //UILabel *label = (UILabel *)touch.view;
        //if (dragging) {
            //CGRect frame = label.frame;
            //frame.origin.x = label.frame.origin.x + touchLocation.x - oldX;
            //frame.origin.y = label.frame.origin.y + touchLocation.y - oldY;
            //label.frame = frame;
        //}
    //}
}
- (void) mouseUp:(UIEvent *)event {
    
}


@end
