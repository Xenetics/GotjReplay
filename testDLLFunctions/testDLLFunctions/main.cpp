//#include "Replay.h"
#include "was/storage_account.h"
#include "was/table.h"

#define ACCOUNT "gotjreplay"
#define KEY "4ZN8dh2gHVdmLzEJ4KFyHOr87i98xyn/efSppFNX2iRWNwIcQ6WT5t19frSFEL0+FmdBrReOr4lax6D8BvuKvw=="

// Connection String, Used for authentication with Azure
utility::string_t storage_connection_string;

// Current connected table client
azure::storage::cloud_table_client table_client;

// Uses Storage access key to Connect to the storage account name given
// Returns a boolean, true if successful
bool Connect(char *storage_name, char *key);

// Creates a table with provided string for the name
// Returns a boolean, true if successful
bool CreateReplay(char *replay_id, char *replay_data);

// Deletes the replay with name matching string provided
// Returns a boolean, true if successful
bool DeleteReplay(char *replay_id);

// Retrieves a replay with a given Replay ID
// Returns the replay data in a Char array which you will have to parse the way you want
char* RetrieveReplay(char *replay_id);

// Retrieves multiple replay from a start point to the range amount
// Returns a char array with all the replay datas
char* RetrieveRangeReplay(int start, int range);

// Retrieves a list of replays wo display
// Returns a char array list of replays
char* RetrieveListOfReplays(int start, int range);


int main()
{
	char *account(const_cast<char*>(ACCOUNT));
	char *key(const_cast<char*>(KEY));
	

	bool connected = Connect(account, key);
	std::cout << connected;
	//std::cout << CreateReplay(const_cast<char*>("Testies012345678910"), const_cast<char*>("1098142825%10.64814$(-0.1,7.5)#(0.0,0.0)#"));
	//char *list = RetrieveListOfReplays(0, 0);
	//std::cout << list;
	//char *replayid = "ReplayIDBIGGEST";
	//char *replay = RetrieveReplay(replayid);
	//std::cout << replay;
	DeleteReplay(const_cast<char*>("Testies012345678910"));
	std::cin.get();
	return 0;
}

///<summary> Uses Storage access key to Connect </summary>
bool Connect(char *storage_name, char *key)
{
	try
	{
		// Builds Connection string for Azure
		storage_connection_string = U("DefaultEndpointsProtocol=https;AccountName=")
									+ utility::conversions::to_string_t(storage_name)
									+ U(";AccountKey=")
									+ utility::conversions::to_string_t(key)
									+ U(";");

		// Retrieve storage account
		azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(storage_connection_string);

		// Create table client
		table_client = storage_account.create_cloud_table_client();

		return true;
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error: ") << e.what() << std::endl;

		return false;
	}
}

///<summary >Uses a table name passed in to attempt to create a new one </summary>
bool CreateReplay(char *replay_id, char *replay_data)
{
	try
	{
		// Retrieve refference to a table
		azure::storage::cloud_table table = table_client.get_table_reference(utility::conversions::to_string_t(replay_id));

		// Create table if it does not exist
		table.create_if_not_exists();

		// Creates a replay
		azure::storage::table_entity replay(U("Replay"), utility::conversions::to_string_t(replay_id));

		// Creates properties for replay
		azure::storage::table_entity::properties_type& properties = replay.properties();

		// Reserves how many property slots you want
		properties.reserve(2);

		// Sets a property called Replay and adds the data to it
		properties[U("ReplayData")] = azure::storage::entity_property(utility::conversions::to_string_t(replay_data));

		// Sets a property called Likes and adds the data to it
		properties[U("Likes")] = azure::storage::entity_property(U("0"));

		// Creates an operation to add the entity to azure
		azure::storage::table_operation add_replay = azure::storage::table_operation::insert_entity(replay);

		// Executes the operation
		azure::storage::table_result add_result = table.execute(add_replay);

		return true;
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error: ") << e.what() << std::endl;

		return false;
	}
}

///<summary> Uses a provided ID to delete a replay </summary>
bool DeleteReplay(char *replay_id)
{
	try
	{
		// Retrieve refference to a table
		azure::storage::cloud_table table = table_client.get_table_reference(utility::conversions::to_string_t(replay_id));

		// Deletes the retrieved replay
		table.delete_table_if_exists_async();

		return true;
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error: ") << e.what() << std::endl;

		return false;
	}
}

///<summary> Uses a provided Replay ID to retrieve a specific replay from azure </summary>
char* RetrieveReplay(char *replay_id)
{
	char *replay_data;
	try
	{
		// Retrieve refference to a table
		azure::storage::cloud_table table = table_client.get_table_reference(utility::conversions::to_string_t(replay_id));

		//Retrieve Entity
		azure::storage::table_operation retrievalOP = azure::storage::table_operation::retrieve_entity(U("Replay"), utility::conversions::to_string_t(replay_id));
		azure::storage::table_result retrievalResult = table.execute(retrievalOP);

		// Get property in ReplayData property on the entity in the table
		utility::string_t replay_string = retrievalResult.entity().properties().at(U("ReplayData")).string_value();

		// instantiate the array to size of ReplayData string
		replay_data = new char[replay_string.length()];

		// Null terminate at the last point in the array
		replay_data[replay_string.length()] = '\0';

		// Build the char array out of the Replay Data
		for (int i = 0; i < replay_string.length(); ++i)
		{
			replay_data[i] = replay_string[i];
		}

		return replay_data;
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error: ") << e.what() << std::endl;

		replay_data = new char[0];
		return replay_data;
	}
}

///<summary> Uses a provided start point and to give back replays based on the range </summary>
char* RetrieveRangeReplay(int start, int range)
{
	char *replay_list;
	try
	{
		// Retrieve list of tables
		azure::storage::result_segment<azure::storage::cloud_table> tables = table_client.list_tables_segmented(tables.continuation_token());

		int array_size = 0;
		for each(azure::storage::cloud_table table in tables.results())
		{
			array_size += table.name().size();
		}

		// instantiate the array to size of table list string
		replay_list = new char[array_size + tables.results().size()];

		// Null terminate at the last point in the array
		replay_list[array_size + tables.results().size()] = '\0';

		int i = 0;
		for (int k = start; k < (start + range); ++k)
		{
			for (int j = 0; j < tables.results()[k].name().size(); j++)
			{
				replay_list[i] = tables.results()[k].name()[j];
				i++;
			}
			replay_list[i] = '@';
		}

		return replay_list;
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error: ") << e.what() << std::endl;

		replay_list = new char[0];
		return replay_list;
	}
}

///<summary> Uses a provided start point and to give back replays based on the range </summary>
char* RetrieveListOfReplays(int start, int range)
{
	char *replay_list;
	try
	{
		// Retrieve list of tables
		azure::storage::result_segment<azure::storage::cloud_table> tables = table_client.list_tables_segmented(tables.continuation_token());

		int array_size = 0;
		for each(azure::storage::cloud_table table in tables.results())
		{
			array_size += table.name().size();
		}

		// instantiate the array to size of table list string
		replay_list = new char[array_size + tables.results().size()];

		// Null terminate at the last point in the array
		replay_list[array_size + tables.results().size()] = '\0';

		// Build the char array out of the Table List
		// i is for place in array
		// j is char in name
		int i = 0;
		for each(azure::storage::cloud_table table in tables.results())
		{
			for (int j = 0; j < table.name().size(); j++)
			{
				replay_list[i] = table.name()[j];
				i++;
			}
			replay_list[i] = '@';
			i++;
		}
		return replay_list;
	}
	catch (const std::exception& e)
	{
		std::wcout << U("Error: ") << e.what() << std::endl;

		replay_list = new char[0];
		return replay_list;
	}
}