# Drupal 6 tests backport from D7

- Backported core tests:
  - Bootstrap (almost all bootstrap functions are from D7)
  - Module system
  - Common functions
  - Default caching system
  - Form API tests
  - Session tests
  - Error tests
  - File tests
  - Image tests
  - Locking test
  - Mail test (convert html to text only)
  - Menu test
  - Pager test
  - Path test
  - Schema API test
  - Tablesort test
  - Theme test

- Backported modules tests:
  - Aggregator
  - Blocks (full version from D7)
  - Blog
  - Book
  - Color
  - Comment
  - Contact
  - Dblog
  - Filter
  - Forum
  - Help
  - Locale
  - Menu
  - Node
  - Path
  - PHP
  - Poll
  - Profile

Module tests on the way:
 - Search
 - Statistics
 - Syslog
 - System
 - Taxonomy
 - Throttle
 - Tracker
 - Translation
 - Trigger
 - Update
 - Upload
 - User

Modules without tests:
 - OpenID (not significant)
 - Ping (not present in D7)