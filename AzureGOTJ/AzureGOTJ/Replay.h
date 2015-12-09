#ifdef REPLAY_EXPORT
#define REPLAY_API __declspec(dllexport) 
#else
#define REPLAY_API __declspec(dllimport) 
#endif

#include "was/storage_account.h"
#include "was/table.h"

extern "C"
{
	// Connection String, Used for authentication with Azure
	utility::string_t storage_connection_string;

	// Current connected table client
	azure::storage::cloud_table_client table_client;

	// Uses Storage access key to Connect to the storage account name given
	// Returns a boolean, true if successful
	REPLAY_API bool Connect(char *storage_name, char *key);

	// Creates a table with provided string for the name
	// Returns a boolean, true if successful
	REPLAY_API bool CreateReplay(char *replay_id, char *replay_data);

	// Deletes the replay with name matching string provided
	// Returns a boolean, true if successful
	REPLAY_API bool DeleteReplay(char *replay_id);

	// Retrieves a replay with a given Replay ID
	// Returns the replay data in a Char array which you will have to parse the way you want
	REPLAY_API char* RetrieveReplay(char *replay_id);

	// Retrieves multiple replay from a start point to the range amount
	// Returns a char array with all the replay datas
	REPLAY_API char* RetrieveRangeReplay(int start, int range);

	// Retrieves a list of replays wo display
	// Returns a char array list of replays
	REPLAY_API char* RetrieveListOfReplays();

	// Uses a provided Replay ID to retrieve a specific replay from azure and add 1 like to it
	// Returns a bool to confirm complete
	REPLAY_API bool LikeReplay(char *replay_id);
}

