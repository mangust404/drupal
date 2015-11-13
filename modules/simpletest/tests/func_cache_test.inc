<?php

function func_cache_test_setter_func_different_file($arg = NULL) {
  global $test_data;
  if(!empty($arg)) {
    return $test_data . '_' . $arg;
  } else {
    return $test_data;
  }
}