<?php

/**
 * @file
 *
 * Functions to abstract this modules database usage.
 */

define('TABLE_ISLANDORA_AUTOCOMPLETE_SPARQL', 'islandora_autocomplete_sparql');

/**
 * Performs a query and retrieves all
 * the Autocomplete URL's stored in the database.
 *
 * @return object
 *   A database query result resource, or FALSE if the query was not
 *   executed correctly.
 */
function islandora_autocomplete_sparql_db_get_sparql_fields() {
  return db_query('SELECT * FROM {%s}', TABLE_ISLANDORA_AUTOCOMPLETE_SPARQL);
}

/**
 * Fetches the given Autocomplete URL from the database.
 *
 * @param int $url_id
 *   The unique id for the requested Autocomplete URL.
 *
 * @return object
 *   An object representing the given autocomplete url
 *   sparql source identified by name, or FALSE.
 *   The attributes of this object are the table fields selected by the query.
 */
function islandora_autocomplete_sparql_db_get_url_source($url_id) {
  $result = db_query('SELECT * FROM {%s} WHERE url_id = %d', TABLE_ISLANDORA_AUTOCOMPLETE_SPARQL, $url_id);
  return $result !== FALSE ? db_fetch_object($result) : FALSE;
}

/**
 * Adds a new autocomplete URL if one by the same name doesn't already exist.
 *
 * @param int $url_id
 *   The unique id for the requested Autocomplete URL.
 * @param array $values
 *   the values to add to the database.
 *
 * @return boolean
 *   TRUE if successful FALSE otherwise.
 */
function islandora_autocomplete_sparql_db_add_url_source($url_id, $values) {
  extract($values, EXTR_SKIP);
  if (db_result(db_query('SELECT count(id) FROM {%s} WHERE url_id = %d', TABLE_ISLANDORA_AUTOCOMPLETE_SPARQL, $url_id)) == 0) {
    $object = new stdClass();
    $object->url_id = $url_id;
    $object->sparql_field = $sparql_field ? trim($sparql_field) : '';
    $object->rdf_relationship = $rdf_relationship ? trim($rdf_relationship) : '';
    $object->object_pid_append_required = $object_pid_append_required;
    $object->subject_dsid = $subject_dsid ? trim($subject_dsid) : '';
    $object->subject_namespace_info = $subject_namespace_info ? trim($subject_namespace_info) : '';
    $object->subject_xpath_data_search = $subject_xpath_data_search ? trim($subject_xpath_data_search) : '';
    $object->object_creation_required = $object_creation_required;
    $object->inverse_required = $inverse_required;
    $object->symmetry_in_object_datastream_required = $symmetry_in_object_datastream_required;
    $object->object_dsid = $object_dsid ? trim($object_dsid) : '';
    $object->object_namespace_info = $object_namespace_info ? $object_namespace_info : '';
    $object->object_datastream_xml = $object_datastream_xml ? trim($object_datastream_xml) : '';
    $object->object_xpath_title_search = $object_xpath_title_search ? trim($object_xpath_title_search) : '';
    $object->object_xpath_date_create_search = $object_xpath_date_create_search ? trim($object_xpath_date_create_search) : '';
    $object->object_content_model_pid = $object_content_model_pid ? trim($object_content_model_pid) : '';
    $object->object_collection_pid = $object_collection_pid ? trim($object_collection_pid) : '';
    $object->object_namespace_prefix = $object_namespace_prefix ? trim($object_namespace_prefix) : '';
    $object->object_transform = $object_transform ? trim($object_transform) : '';
    $object->object_state = $object_state ? trim($object_state) : '';
    $object->object_xpath_data_search = $object_xpath_data_search ? trim($object_xpath_data_search) : '';
    $object->object_xpath_data_build = $object_xpath_data_build ? trim($object_xpath_data_build) : '';
    $object->object_xpath_data_build_countback = $object_xpath_data_build_countback ? trim($object_xpath_data_build_countback) : '';

    return drupal_write_record(TABLE_ISLANDORA_AUTOCOMPLETE_SPARQL, $object) !== FALSE;
  }
  return FALSE;
}

/**
 * Update the given Autocomplete URL.
 *
 * @param int $id
 *   The unique ID for the record we want to update.
 * @param int $url_id
 *   The unique id for the requested Autocomplete URL.
 * @param array $values
 *   the values to add to the database.
 *
 * @return boolean
 *   TRUE if successful FALSE otherwise.
 */
function islandora_autocomplete_sparql_db_update_url_source($id, $url_id, $values) {
  extract($values, EXTR_SKIP);
  $source = islandora_autocomplete_sparql_db_get_url_source($url_id);
  if ($source !== FALSE) {
    $object = new stdClass();
    $object->id = $id;
    $object->url_id = $url_id;
    $object->sparql_field = $sparql_field ? trim($sparql_field) : '';
    $object->rdf_relationship = $rdf_relationship ? trim($rdf_relationship) : '';
    $object->object_pid_append_required = $object_pid_append_required;
    $object->subject_dsid = $subject_dsid ? trim($subject_dsid) : '';
    $object->subject_namespace_info = $subject_namespace_info ? trim($subject_namespace_info) : '';
    $object->subject_xpath_data_search = $subject_xpath_data_search ? trim($subject_xpath_data_search) : '';
    $object->object_creation_required = $object_creation_required;
    $object->inverse_required = $inverse_required;
    $object->symmetry_in_object_datastream_required = $symmetry_in_object_datastream_required;
    $object->object_dsid = $object_dsid ? trim($object_dsid) : '';
    $object->object_namespace_info = $object_namespace_info ? $object_namespace_info : '';
    $object->object_datastream_xml = $object_datastream_xml ? trim($object_datastream_xml) : '';
    $object->object_xpath_title_search = $object_xpath_title_search ? trim($object_xpath_title_search) : '';
    $object->object_xpath_date_create_search = $object_xpath_date_create_search ? trim($object_xpath_date_create_search) : '';
    $object->object_content_model_pid = $object_content_model_pid ? trim($object_content_model_pid) : '';
    $object->object_collection_pid = $object_collection_pid ? trim($object_collection_pid) : '';
    $object->object_namespace_prefix = $object_namespace_prefix ? trim($object_namespace_prefix) : '';
    $object->object_transform = $object_transform ? trim($object_transform) : '';
    $object->object_state = $object_state ? trim($object_state) : '';
    $object->object_xpath_data_search = $object_xpath_data_search ? trim($object_xpath_data_search) : '';
    $object->object_xpath_data_build = $object_xpath_data_build ? trim($object_xpath_data_build) : '';
    $object->object_xpath_data_build_countback = $object_xpath_data_build_countback ? trim($object_xpath_data_build_countback) : '';

    return drupal_write_record(TABLE_ISLANDORA_AUTOCOMPLETE_SPARQL, $object, 'id');
  }
  return FALSE;
}

/**
 * Deletes the given Autocomplete URL.
 *
 * @param int $url_id
 *   The unique id of the URL to delete.
 */
function islandora_autocomplete_sparql_db_delete_url_source($url_id) {
  $source = islandora_autocomplete_sparql_db_get_url_source($url_id);
  if ($source !== FALSE) {
    file_delete($source->sparql_field);
    db_query('DELETE FROM {%s} WHERE id = "%d"', TABLE_ISLANDORA_AUTOCOMPLETE_SPARQL, $source->id);
  }
}
