DROP TABLE IF EXISTS Scene;
CREATE TABLE Scene (
  id INTEGER PRIMARY KEY,
  portraitWallpaper TEXT,
  landscapeWallpaper TEXT);

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

DROP TABLE IF EXISTS SnapConfiguration;
CREATE TABLE SnapConfiguration (
	snappingEnabled INTEGER,
	showEffects INTEGER,
	snapForce REAL,
	snapGap REAL,
	borderGap REAL,
	timeout INTEGER);
	
DROP TABLE IF EXISTS FeedbackConfiguration;
CREATE TABLE FeedbackConfiguration (
  pageChangeFeedbackType TEXT,
  widgetPickFeedbackType TEXT, 
  widgetDropFeedbackType TEXT,
  widgetRepositionFeedbackType TEXT,
  widgetOverTrashbinFeedbackType TEXT,
  widgetDropToTrashbinFeedbackType TEXT,
  shortcutWidgetTapFeedbackType TEXT,
  widgetMoveBlockedFeedbackType TEXT,
  clockWidgetTapFeedbackType TEXT,
  widgetSnappingFeedbackType TEXT);

DROP TABLE IF EXISTS GeneralConfiguration;
CREATE TABLE GeneralConfiguration (
  bounceEffect INTEGER, 
  tapAndHoldDistance REAL, 
  widgetTapAndHoldTimeout INTEGER,
  sceneTapAndHoldTimeout INTEGER,  
  pageChangeZoneWidth REAL,
  pageIndicatorSpacing REAL,
  pageChangeAnimationDuration INTEGER,
  pageChangeZoneAnimationDuration INTEGER,
  pageChangeZoneReverseAnimationDuration INTEGER,
  pageRemovedAnimationDuration INTEGER, 
  newPageAddedAnimationDuration INTEGER, 
  widgetDragEffectDuration INTEGER, 
  widgetDropEffectDuration INTEGER,
  boundaryFeedbackEffectDistance INTEGER,
  defaultPageId INTEGER,
  maximumPageCount INTEGER,
  maximumWidgetHeight REAL,
  maximumWidgetWidth REAL,
  minimumWidgetHeight REAL,
  minimumWidgetWidth REAL,
  shortcutLabelsVisible INTEGER,
  pageChangePanDistance REAL);
 