#import <Cocoa/Cocoa.h>
#import <QuartzCore/CVDisplayLink.h>

@interface View : NSOpenGLView {
	CVDisplayLinkRef displayLink;
}

@end
