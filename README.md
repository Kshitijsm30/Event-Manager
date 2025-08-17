Smart Event Manager (C++ CLI)
Smart Event Manager is a C++ command-line application for scheduling, managing, and viewing events with persistent storage.

Key Features

Admin: Add, edit, delete, search events; view summary by type; export reports; simulate reminders.

Guest: View and search events; daily and today’s event views.

Persistent Storage: Events are saved in events.txt and loaded on program start.

Functions Overview

Event Management
addEventInteractive(): Adds new event with conflict detection.
editEventInteractive(): Edit event fields selectively; checks conflicts.
deleteEventInteractive(): Remove an event by ID.
viewAllEvents(): Lists all events sorted by date/time.
searchEventsInteractive(): Search events by name/type (case-insensitive).
summaryByType(): Counts events grouped by type.
dayView(date): Shows events for a specific date.
todaysEvents(): Displays today’s events.
sendRemindersCLI(): Simulates reminders for a given date.

Data Handling
loadEvents(): Loads events from events.txt.
saveEvents(): Saves current events to file.
exportReport(): Exports sorted events to report.txt.

Exception & Validation Handling
Date Validation: Ensures correct format DD-MM-YYYY, valid month/day, leap years.
Conflict Detection: Warns or prevents scheduling conflicts.
Input Safety: Handles leftover input in streams with cin.ignore.
File Operations: Checks file existence and handles errors when reading/writing.


Utility
getTodayDate(): Returns current date.
isValidDate(date): Validates date format and correctness (leap years included).
hasConflict(date, time, excludeId): Checks for overlapping events.
toLower(string): Converts string to lowercase for searches.


Menus
adminMenu(): Admin interface with all management options.
guestMenu(): Guest interface (read-only).
main(): Entry point; handles role selection, admin authentication, and exits.


Admin password: admin123
