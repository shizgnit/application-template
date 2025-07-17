#import "Window.h"
#import "AppDelegate.h"

@implementation Window

-(instancetype)init
{
	// Create a screen-sized window on the display you want to take over
	NSRect screenRect = [[NSScreen mainScreen] frame];

	// Initialize the window making it size of the screen and borderless
	self = [super initWithContentRect:screenRect
							styleMask:NSBorderlessWindowMask
							  backing:NSBackingStoreBuffered
								defer:YES];

	// Set the window level to be above the menu bar to cover everything else
	[self setLevel:NSMainMenuWindowLevel+1];

	// Set opaque
	[self setOpaque:YES];

	// Hide this when user switches to another window (or app)
	[self setHidesOnDeactivate:YES];

	return self;
}

-(BOOL)canBecomeKeyWindow
{
	// Return yes so that this borderless window can receive input
	return YES;
}

- (void)keyDown:(NSEvent *)event
{
	[[self windowController] keyDown:event];
    [Application keyDown:event];
}

- (void)keyUp:(NSEvent *)event
{
    [[self windowController] keyUp:event];
    [Application keyUp:event];
}

- (void)mouseDown:(NSEvent *)event
{
    [self setAcceptsMouseMovedEvents:YES];
    [Application mouseDown:event];
}

- (void)mouseMoved:(NSEvent *)event
{
    [Application mouseMoved:event];
}

- (void)mouseDragged:(NSEvent *)event
{
    [Application mouseDragged:event];
}

- (void)mouseUp:(NSEvent *)event
{
    [Application mouseUp:event];
}

- (void)scrollWheel:(NSEvent *)event
{
    [Application scrollWheel:event];
}

@end

