
#ifndef CollisionHandler_hpp
#define CollisionHandler_hpp
class Boat;
class Soul;

typedef enum entityType{WallType, BoatType, SoulType, FinishLineType} EType;

class CollisionHandler {
public:
	CollisionHandler(EType other) : type(other){ }
	EType type;
	virtual void handleCollision(EType other) = 0;
};

class BoatCollisionHandler : CollisionHandler {
	Boat *parent;
public:
	BoatCollisionHandler(Boat *p) : parent(p), CollisionHandler(BoatType) {}
	void handleCollision(EType other);
};

class SoulCollisionHandler : CollisionHandler {
	Soul *parent;
public:
	SoulCollisionHandler(Soul *s) : parent(s), CollisionHandler(SoulType) {}
	void handleCollision(EType other);
};

#endif /* CollisionHandler_hpp */
