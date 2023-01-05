#pragma once
#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 30.f, 12, FColor::Red, true)
#define DRAW_SPHERE_Color(Location, Color) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 8.f, 12, Color, false, 5.f)
#define DRAW_SPHERE_SingleFrame(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 30.f, 12, FColor::Red, false, -1.f)
#define DRAW_LINE(StartLoc, EndLoc) if(GetWorld()) DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, true, -1.f, 0, 1.f)
#define DRAW_LINE_SingleFrame(StartLoc, EndLoc) if(GetWorld()) DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, false, -1.f, 0, 1.f)
#define DRAW_POINT(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, true)
#define DRAW_POINT_SingleFrame(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.f, FColor::Red, false, -1.f)
#define DRAW_VECTOR(StartLoc, EndLoc) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, true, -1.f, 0, 1.f); \
		DrawDebugPoint(GetWorld(), EndLoc, 15.f, FColor::Red, true); \
	}
#define DRAW_VECTOR_SingleFrame(StartLoc, EndLoc) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, false, -1.f, 0, 1.f); \
		DrawDebugPoint(GetWorld(), EndLoc, 15.f, FColor::Red, false, -1.f); \
	}

#define PRINT_ON_SCREEN(Message) if (GEngine) \
	{ \
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Purple, Message); \
	}