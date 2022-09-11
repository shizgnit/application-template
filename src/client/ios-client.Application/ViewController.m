//
//  ViewController.m
//  ios-client.Application
//
//  Created by Dee Abbott on 7/31/22.
//

#import "ViewController.h"
#import "AppDelegate.h"

@interface ViewController ()
@property (strong, nonatomic) EAGLContext *context;

@end

@implementation ViewController
{
    bool started;
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
    
    [EAGLContext setCurrentContext:self.context];
    
    started = false;
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
    if(started == false) {
        CGRect window = self.view.bounds;
        [Application resize:&window];
        started = true;
    }
    [Application draw];
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    NSLog(@"began type: %i", (int)event.type);
    NSLog(@"count: %i", (int)event.allTouches.count);

    self.paused = !self.paused;
    
    [Application mouseDown:event];
    
    NSLog(@"timeSinceLastUpdate: %f", self.timeSinceLastUpdate);
    NSLog(@"timeSinceLastDraw: %f", self.timeSinceLastDraw);
    NSLog(@"timeSinceFirstResume: %f", self.timeSinceFirstResume);
    NSLog(@"timeSinceLastResume: %f", self.timeSinceLastResume);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    NSLog(@"moved type: %i", (int)event.type);
    NSLog(@"count: %i", (int)event.allTouches.count);
    
    [Application mouseDragged:event];
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
    [super pressesBegan: presses withEvent: event];
    [Application keyDown:event];
 }

 - (void)pressesEnded:(NSSet<UIPress *> *)presses withEvent:(UIPressesEvent *)event {
    [super pressesEnded: presses withEvent: event];
    [Application keyUp:event];
 }
@end
