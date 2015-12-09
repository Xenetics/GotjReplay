#include "Replay.h"
#include "was/storage_account.h"
#include "was/table.h"

extern "C"
{
	///<summary> Uses Storage access key to Connect </summary>
	bool Connect(char *storage_name, char *key)
	{
		try
		{
			// Builds Connection string for Azure
			storage_connection_string =	U("DefaultEndpointsProtocol=https;AccountName=") 
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
	char* RetrieveListOfReplays()
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

	///<summary> Uses a provided Replay ID to retrieve a specific replay from azure and add 1 like to it </summary>
	bool LikeReplay(char *replay_id)
	{
		try
		{
			// Retrieve refference to a table
			azure::storage::cloud_table table = table_client.get_table_reference(utility::conversions::to_string_t(replay_id));

			//Retrieve Entity
			azure::storage::table_operation retrievalOP = azure::storage::table_operation::retrieve_entity(U("Replay"), utility::conversions::to_string_t(replay_id));
			azure::storage::table_result retrievalResult = table.execute(retrievalOP);

			// Get property in ReplayData property on the entity in the table
			utility::string_t replay_string = retrievalResult.entity().properties().at(U("ReplayData")).string_value();

			// Get property in ReplayData property on the entity in the table
			utility::string_t likes_string = retrievalResult.entity().properties().at(U("Likes")).string_value();

			// Replace an entity.
			azure::storage::table_entity entity_to_replace(U("Replay"), utility::conversions::to_string_t(replay_id));
			azure::storage::table_entity::properties_type& properties_to_replace = entity_to_replace.properties();
			properties_to_replace.reserve(2);

			// Sets a property called Replay and adds the data to it
			properties_to_replace[U("ReplayData")] = azure::storage::entity_property(utility::conversions::to_string_t(replay_string));

			int count(std::stoi(likes_string));
			count++;
			// Sets a property called Likes and adds the data to it
			properties_to_replace[U("Likes")] = azure::storage::entity_property(utility::conversions::to_string_t(std::to_string(count)));

			// Create an operation to replace the entity.
			azure::storage::table_operation replace_operation = azure::storage::table_operation::replace_entity(entity_to_replace);

			// Submit the operation to the Table service.
			azure::storage::table_result replace_result = table.execute(replace_operation);

			return true;
		}
		catch (const std::exception& e)
		{
			std::wcout << U("Error: ") << e.what() << std::endl;

			return false;
		}
	}
}