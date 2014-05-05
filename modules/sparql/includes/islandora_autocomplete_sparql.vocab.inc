
<?php

/**
 * @file
 *
 * These functions were lifted from the stand-alone Islandora Relationship
 * Editor Module. They were given the prefix "fba_" to avoid clashes if
 * that module is loaded.
 *
 * Functions for rendering/vaildating/submitting the Sparql subsection of the
 * autocomplet URL form.
 */

function fba_has_inverseof_value($value_relationship) {
  if (!empty($value_relationship)) {
  	$vocabulary = fba_get_current_vocabulary();
  	$term = $vocabulary['terms'][$value_relationship];
	if (isset($term['inverse'][0])) {
		return TRUE;
	}
  }

  return FALSE;
}

function fba_get_possible_relationships($object_pid) {

  $list_term = array();
  $vocabulary = fba_get_current_vocabulary();
  if (count($vocabulary) > 0) {
    foreach ($vocabulary['terms'] as $term) {
      list($term_ns, $term_local_name) = fba_rdfx_split_uri($term['uri']);
      if (isset($term['label'])) {
        if (isset($term['label']['en'])) {
          $label = $term['label']['en'];
        }
        else {
          $label = reset($term['label']);
        }
      }
      else {
        $label = "";
      }
      $list_term[$term['uri']] = t($label);
    }
    array_multisort($list_term, SORT_ASC);
    array_unshift($list_term, '--- Select ---');
  }
  return $list_term;
}

function fba_create_vocabulary() {
  include_once('arc2/ARC2.php');
  module_load_include('inc', 'fedora_repository', 'api/tuque');

  $pid = variable_get('islandora_relationship_editor_ontology_pid', t('fba:ontology'));
  $dsid = variable_get('islandora_relationship_editor_ontology_dsid', t('ONTOLOGY'));
  $fedora_object = islandora_object_load($pid);
  if (!$fedora_object) {
    return (FALSE);
  }
  $datastream = $fedora_object[$dsid];
  if (!$datastream) {
    return (FALSE);
  }
  $namespaces = array();
  $parser = ARC2::getRDFParser();
  $fedora_url = variable_get('islandora_base_url', 'http://localhost:8080/fedora');
  $xmlfilelink = $fedora_url . "/objects/" . $pid . "/datastreams/" . $dsid . "/content";
  $parser->parse($xmlfilelink);
  $triples = $parser->getTriples();
  $terms = fba__rdfx_fetch_terms($triples);
  foreach ($parser->parser->nsp as $uri => $prefix) {
    $namespaces[$prefix] = $uri;
  }
  $vocabulary = array(
    'terms' => $terms,
    'namespaces' => $namespaces,
  );
  return ($vocabulary);
}

function fba_get_current_vocabulary() {
  //SFOX this is d7 code...$ireo_voc = &drupal_static(__FUNCTION__); changed it the line below
  static $ireo_voc;

  if (!isset($ireo_voc)) {
    //SFOX commented this out for now.
//    if ($cache = cache_get('islandora_relationship_editor_ontology_voc')) {
//      $ireo_voc = $cache->data;
//    }
//    else {
      $ireo_voc = fba_create_vocabulary();
      cache_set('islandora_relationship_editor_ontology_voc', $ireo_voc, 'cache');
//    }
  }
  return $ireo_voc;
}

function fba__rdfx_query_find_literal(&$model, $queries) {
  $literal = array();
  foreach ($queries as $query) {
    list($s, $p, $o) = $query;
    $triples = fba__rdfx_query_find_all($model, $s, $p, $o);
    // We create an associative array based on the language code of the
    // literal. The language codes Drupal uses are specified in includes/iso.inc.
    foreach ($triples as $triple) {
      if ($triple['o_lang'] !== '') {
        // Chinese and Portuguese are the only languages with a >2 letter
        // langcode.
        if (preg_match('/(zh-hans)|(zh-hant)|(pt-pt)|(pt-br)/', $triple['o_lang'])) {
          $langcode = $triple['o_lang'];
        }
        // Remove the region code if it is used (i.e. en-US is changed to en).
        else {
          $lang_array = explode('-', $triple['o_lang']);
          $langcode = !empty($lang_array[0]) ? $lang_array[0] : $triple['o_lang'];
        }
      }
      else {
        $langcode = 'und';
      }
      $literal[$langcode] = $triple['o'];
    }
  }
  return $literal;
}

function fba__rdfx_query_find_all(&$model, $s, $p, $o) {
  $result = array();
  foreach ($model as $triple) {
    if (!is_null($s) && $s != '?' && ($triple['s'] != $s))
      continue;
    if (!is_null($p) && $p != '?' && ($triple['p'] != $p))
      continue;
    if (!is_null($o) && $o != '?' && ($triple['o'] != $o))
      continue;
    $result[] = $triple;
  }
  return $result;
}

function fba__rdfx_fetch_terms(&$model) {
  $terms = array();
  $term_uris = array();

  // Retrieve the queries for term retrieval.
  $term_type_group = fba_iowl_term_types();

  $property = $term_type_group['term_types']['property'];
  $query = array(
    array('?', 'http://www.w3.org/1999/02/22-rdf-syntax-ns#type', $property['uri']),
  );
  $term_uris = fba__rdfx_query_find_uris($model, $query);


  // Add term details and various relationships for each term, as defined
  // in rdfx_term_types() and altered by hook_rdfx_term_types_alter().
  $query_x = array();
  foreach ($term_uris as $term_uri) {
    $terms[$term_uri] = fba__evoc_query_for_term_description($model, $term_uri);
    foreach ($term_type_group['description'] as $property => $queries) {
      foreach ($queries as $predicate_uri => $query_types) {
        foreach ($query_types as $query_type) {
          switch ($query_type) {
            case 'subject':
              $query_x[$term_uri][$property][] = array('?', $predicate_uri, $term_uri);
              break;
            case 'object':
              $query_x[$term_uri][$property][] = array($term_uri, $predicate_uri, '?');
              break;
          }
        }
      }
      $terms[$term_uri][$property] = fba__rdfx_query_find_uris($model, $query_x[$term_uri][$property]);
    }
  }
  array_multisort($terms, SORT_ASC);

  return $terms;
}

function fba__rdfx_query_find_uris(&$model, $queries) {
  $uris = array();
  foreach ($queries as $query) {
    list($s, $p, $o) = $query;
    $result = fba__rdfx_query_find_all($model, $s, $p, $o);
    foreach ($result as $triple) {
      if ($s == '?' && $triple['s_type'] == 'uri') {
        $uris[] = $triple['s'];
      }
      if ($p == '?') {
        $uris[] = $triple['p'];
      }
      if ($o == '?' && $triple['o_type'] == 'uri') {
        $uris[] = $triple['o'];
      }
    }
  }
  return array_unique($uris);
}

function fba__evoc_query_for_term_description(&$model, $uri) {
  $label = fba__rdfx_query_find_literal($model, array(
        array($uri, 'http://www.w3.org/2000/01/rdf-schema#label', '?')
      ));

  $comment = fba__rdfx_query_find_literal($model, array(
        array($uri, 'http://www.w3.org/2000/01/rdf-schema#comment', '?'),
      ));

  return array(
    'uri' => $uri,
    'label' => $label,
    'comment' => $comment,
  );
}

function fba_iowl_term_types($reset = FALSE) {
  static $types;
  if ($reset || !isset($types)) {

    $types['term_types'] = array();
    $term_details = '';
    $types['term_types'] = array(
      'property' => array(
        'uri' => 'http://www.w3.org/2002/07/owl#ObjectProperty',
      ),
    );

    $types['description'] = array(
      'superproperty' => array(
        'http://www.w3.org/2000/01/rdf-schema#subPropertyOf' => array(
          'object',
        ),
      ),
      'inverse' => array(
        'http://www.w3.org/2002/07/owl#inverseOf' => array(
          'object',
        ),
      ),
    );
  }
  return $types;
}

function fba_rdfx_split_uri($uri) {
  include_once('arc2/ARC2.php');

  $parts = ARC2::splitURI($uri);
  return $parts;
}

function fba_textfield_inverseof_value($value_relationship) {
  if (!empty($value_relationship)) {
        $vocabulary = fba_get_current_vocabulary();
        $term = $vocabulary['terms'][$value_relationship];
        if (isset($term['inverse'][0])) {
                list($value_ns, $value_inverse) = fba_rdfx_split_uri($term['inverse'][0]);
        }
  }
  return empty($value_inverse) ? "No inverse" : $value_inverse;
}
