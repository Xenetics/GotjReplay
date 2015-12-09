#pragma once

#include "was/storage_account.h"
#include "was/table.h"

class Replay
{
public:
	// Connection String, Used for authentication with Azure
	static utility::string_t storage_connection_string;

	// Current connected table client
	static azure::storage::cloud_table_client table_client;

	// Uses Storage access key to Connect to the storage account name given
	// Returns a boolean, true if successful
	static bool Connect(char *storage_name, char *key);

	// Creates a table with provided string for the name
	// Returns a boolean, true if successful
	static bool CreateReplay(char *replay_id, char *replay_data);

	// Deletes the replay with name matching string provided
	// Returns a boolean, true if successful
	static bool DeleteReplay(char *replay_id);

	// Retrieves a replay with a given Replay ID
	// Returns the replay data in a Char array which you will have to parse the way you want
	static char* RetrieveReplay(char *replay_id);
};