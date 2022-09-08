//
//  ViewController.m
//  ios-client.Application
//
//  Created by Dee Abbott on 7/31/22.
//

#import "ViewController.h"

#import "engine.hpp"
#import "application.hpp"

@interface ViewController ()
@property (strong, nonatomic) EAGLContext *context;

@end

@implementation ViewController
{
    application *instance;
}

- (void)viewDidLoad {
    [super viewDidLoad];
        
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    if(!self.context) {
        NSLog(@"failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    instance = NULL;
    
    [EAGLContext setCurrentContext:self.context];
}

- (void)dealloc
{
    [EAGLContext setCurrentContext:self.context];

    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    if(instance == NULL) {
        instance = new app();
    }
    
    if (instance->started) {
        instance->on_interval();
        instance->on_draw();
    }
    else {
        assets->init();
        assets->set("shader.path", std::string("shaders-gl"));
        assets->set("shader.version", std::string("#version 300 es"));
        instance->dimensions(self.view.bounds.size.width, self.view.bounds.size.height);
        instance->on_startup();
        instance->started = true;
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    NSLog(@"began type: %i", (int)event.type);
    NSLog(@"count: %i", (int)event.allTouches.count);

    self.paused = !self.paused;
    
    NSLog(@"timeSinceLastUpdate: %f", self.timeSinceLastUpdate);
    NSLog(@"timeSinceLastDraw: %f", self.timeSinceLastDraw);
    NSLog(@"timeSinceFirstResume: %f", self.timeSinceFirstResume);
    NSLog(@"timeSinceLastResume: %f", self.timeSinceLastResume);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    NSLog(@"moved type: %i", (int)event.type);
    NSLog(@"count: %i", (int)event.allTouches.count);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    NSLog(@"ended type: %i", (int)event.type);
    NSLog(@"count: %i", (int)event.allTouches.count);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event {
    NSLog(@"cancelled type: %i", (int)event.type);
    NSLog(@"count: %i", (int)event.allTouches.count);
}

- (void)pressesBegan:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event {
    for (UIPress* press in presses) {
        int key = platform::keys[press.key.keyCode].xref;
        NSLog(@"key: 0x%02x, 0x%02x", key, press.key.keyCode);
        gui->raise({ platform::input::KEY, platform::input::DOWN, key, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
        input->raise({ platform::input::KEY, platform::input::DOWN, key, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
    }
    [super pressesBegan: presses withEvent: event];
 }

 - (void)pressesEnded:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event {
     for (UIPress* press in presses) {
         int key = platform::keys[press.key.keyCode].xref;
         gui->raise({ platform::input::KEY, platform::input::UP, key, 0, 0.0f, { 0.0f, 0.0f, 0.0f } }, 0, 0);
         input->raise({ platform::input::KEY, platform::input::UP, key, 1, 0.0f, { 0.0f, 0.0f, 0.0f } });
     }
     [super pressesEnded: presses withEvent: event];
 }
@end
