#include "Replay.h"

///<summary> Uses Storage access key to Connect </summary>
bool Replay::Connect(char *storage_name, char *key)
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
bool Replay::CreateReplay(char *replay_id, char *replay_data)
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
		properties[U("Replay")] = azure::storage::entity_property(utility::conversions::to_string_t(replay_data));

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
bool Replay::DeleteReplay(char *replay_id)
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
char* Replay::RetrieveReplay(char *replay_id)
{
	char *replay_data;
	try
	{
		// Retrieve refference to a table
		azure::storage::cloud_table table = table_client.get_table_reference(utility::conversions::to_string_t(replay_id));

		azure::storage::table_query query;

		query.set_filter_string(azure::storage::table_query::generate_filter_condition(U("PartitionKey"), azure::storage::query_comparison_operator::equal, U("Replay")));

		azure::storage::table_query_iterator iter = table.execute_query(query);

		utility::string_t replay_string = iter->properties().at(U("Replay")).string_value();

		replay_data = new char[replay_string.length()];

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