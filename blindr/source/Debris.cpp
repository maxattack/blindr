#include "Blindr.h"

Blindr::Debris::Debris() : BodyUserdata(btDebris) {
	
}

Blindr::Debris& Blindr::Debris::init() {
	return *this;
}

Blindr::Debris& Blindr::Debris::release() {
	return *this;
}
		
void Blindr::Debris::draw() {

}
