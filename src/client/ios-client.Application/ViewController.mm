//
//  ViewController.m
//  GLESTest2
//
//  Created by Dee Abbott on 7/31/22.
//

#import "ViewController.h"
#import <OpenGLES/ES3/glext.h>

#import "SimpleRenderer.h"

@interface ViewController ()
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;

@end

@implementation ViewController
{
    SimpleRenderer* mCubeRenderer;
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
        mCubeRenderer = new SimpleRenderer();
        mCubeRenderer->UpdateWindowSize(self.view.bounds.size.width, self.view.bounds.size.height);
    }
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    if(mCubeRenderer != NULL) {
        delete mCubeRenderer;
        mCubeRenderer = NULL;
    }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    mCubeRenderer->Draw();
}

@end
