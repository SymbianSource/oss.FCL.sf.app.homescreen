DROP TABLE IF EXISTS Scene;
CREATE TABLE Scene (
  id INTEGER PRIMARY KEY,
  portraitWallpaper TEXT,
  landscapeWallpaper TEXT,
  defaultPageId INTEGER,
  maximumPageCount INTEGER,
  maximumWidgetHeight INTEGER,
  maximumWidgetWidth INTEGER,
  minimumWidgetHeight INTEGER,
  minimumWidgetWidth INTEGER);

DROP TABLE IF EXISTS Pages;
CREATE TABLE Pages (
  id INTEGER PRIMARY KEY,  
  indexPosition INTEGER);

DROP TABLE IF EXISTS Widgets;
CREATE TABLE Widgets (
  id INTEGER PRIMARY KEY,
  uri TEXT,  
  pageId INTEGER);

DROP TABLE IF EXISTS WidgetPresentations;
CREATE TABLE WidgetPresentations (
  key TEXT,
  x REAL,
  y REAL,
  zValue REAL,	
  widgetId INTEGER,
  UNIQUE(widgetId, key) ON CONFLICT REPLACE,
  FOREIGN KEY(widgetId) REFERENCES Widgets(id) ON DELETE CASCADE);

DROP TABLE IF EXISTS WidgetPreferences;
CREATE TABLE WidgetPreferences (
  key TEXT,
  value TEXT,
  widgetId INTEGER,
  UNIQUE(widgetId, key) ON CONFLICT REPLACE,
  FOREIGN KEY(widgetId) REFERENCES Widgets(id) ON DELETE CASCADE);
