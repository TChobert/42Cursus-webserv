#pragma once

struct locationPropertiesFlags {

	bool allowedMethodsSeen;
	bool UploadAuthSeen;

	locationPropertiesFlags(void);
	~locationPropertiesFlags(void);
	void resetFlags(void);
};
