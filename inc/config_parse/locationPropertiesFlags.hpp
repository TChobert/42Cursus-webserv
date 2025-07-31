#pragma once

struct locationPropertiesFlags {

	bool allowedMethodsSeen;

	locationPropertiesFlags(void);
	~locationPropertiesFlags(void);
	void resetFlags(void);
};
