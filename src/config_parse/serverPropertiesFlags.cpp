#include "serverPropertiesFlags.hpp"

serverPropertiesFlags::serverPropertiesFlags(void) : portSeen(false), hostSeen(false), nameSeen(false), rootSeen(false) {}

serverPropertiesFlags::~serverPropertiesFlags(void) {}

void serverPropertiesFlags::resetFlags(void) {
	portSeen = false;
	hostSeen = false;
	rootSeen = false;
	nameSeen = false;
}
