#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include <tr1/unordered_map>
#include "CinderGLSketch.h"

using namespace ci;


extern NSString * kStrNotifyCinderGLVCLoaded;

@interface CinderGLViewController : GLKViewController <UIGestureRecognizerDelegate> {
    CinderGLSketch *mSketch;
    std::tr1::unordered_map< UITouch*, uint32_t > mTouchIds;
    UIPinchGestureRecognizer *gestureRecognizerPinch_;
}

@property (strong, nonatomic) EAGLContext *context;

- (void)setSketch:(CinderGLSketch*)sketch;

- (uint32_t)addTouchToMap:(UITouch*)touch;
- (void)removeTouchFromMap:(UITouch*)touch;
- (uint32_t)findTouchInMap:(UITouch*)touch;
- (std::vector<app::TouchEvent::Touch>)convertTouches:(NSSet*)touches andAddToMap:(BOOL)add orRemoveFromMap:(BOOL)remove;
- (void)updateActiveTouches;
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event;

+ (CinderGLViewController *) appViewController;

@end
