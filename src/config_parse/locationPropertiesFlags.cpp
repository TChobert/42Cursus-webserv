#include "locationPropertiesFlags.hpp"

locationPropertiesFlags::locationPropertiesFlags(void) : allowedMethodsSeen(false) {}

locationPropertiesFlags::~locationPropertiesFlags(void) {}

void locationPropertiesFlags::resetFlags(void) {

	allowedMethodsSeen = false;
}
