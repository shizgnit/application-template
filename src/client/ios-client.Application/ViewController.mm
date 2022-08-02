//
//  ViewController.m
//  GLESTest2
//
//  Created by Dee Abbott on 7/31/22.
//

#import "ViewController.h"
#import <OpenGLES/ES3/glext.h>

#import "SimpleRenderer.h"

#import "engine.hpp"

#import "application.hpp"

@interface ViewController ()
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation ViewController
{
    SimpleRenderer* mCubeRenderer;
    
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
    
    mCubeRenderer = NULL;
    instance = NULL;
    
    [self setupGL];
}

- (void)dealloc
{
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    if(mCubeRenderer == NULL) {
        //mCubeRenderer = new SimpleRenderer();
        //mCubeRenderer->UpdateWindowSize(self.view.bounds.size.width, self.view.bounds.size.height);
    }
    
    if(instance == NULL) {
        instance = new app();
    }
    
    assets->init();
    
    instance->dimensions(self.view.bounds.size.width, self.view.bounds.size.height);
    instance->on_startup();
    instance->started = true;
    
    /*
     NSString *directory = [NSString stringWithFormat:@"Intro"];
     NSArray *intros = [[NSBundle mainBundle] pathsForResourcesOfType:@"png" inDirectory:directory];
     NSLog(@"%@", intros);
     
     
     NSBundle *b = [NSBundle mainBundle];
     NSString *dir = [b resourcePath];
     NSArray *parts = [NSArray arrayWithObjects:
                       dir, @"assets", @"shaders", @"ortho2d.vert", (void *)nil];
     NSString *path = [NSString pathWithComponents:parts];
     const char *cpath = [path fileSystemRepresentation];
     std::string vertFile(cpath);
     std::ifstream vertFStream(vertFile);
     
     
     NSURL *filepath = [[NSBundle mainBundle] URLForResource:@"files" withExtension:@"plist"];
     NSMutableDictionary *allXCAssetsImages = [NSMutableDictionary dictionaryWithContentsOfFile:filepath.path];
     */
    
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    //if(mCubeRenderer != NULL) {
    //    delete mCubeRenderer;
    //    mCubeRenderer = NULL;
    //}
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    //mCubeRenderer->Draw();
    
    if (instance->started) {
        instance->on_interval();
        instance->on_draw();
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

@end
