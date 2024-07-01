// Himanshu

#ifndef configM_LOG_LVL
#error "configM_LOG_LVL is not defined"
#endif

#define L_BOLD "\033[1m"
#define L_ITALIC "\033[3m"
#define L_UNDERLINE "\033[4m"
#define L_BACKGROUND "\033[7m"
#define L_STRIKETHROUGH "\033[9m"
#define L_MOVE_CURSOR_UP "\033[A"
#define L_MOVE_CURSOR_DOWN "\033[B"
#define L_MOVE_CURSOR_RIGHT "\033[C"
#define L_MOVE_CURSOR_LEFT "\033[D"
#define L_MOVE_CURSOR_NEXT_LINE "\033[E"
#define L_MOVE_CURSOR_PREVIOUS_LINE "\033[F"
#define L_MOVE_CURSOR_COLUMN "\033[%dG"
#define L_MOVE_CURSOR_POSITION "\033[%d;%dH"
#define L_CLEAR_SCREEN "\033[2J"
#define L_CLEAR_LINE "\033[2K"
#define L_CLEAR_LINE_TO_END "\033[K"
#define L_CLEAR_LINE_TO_START "\033[1K"
#define L_CLEAR_SCREEN_TO_END "\033[J"
#define L_CLEAR_SCREEN_TO_START "\033[1J"
#define L_CLEAR_SCREEN_ALL "\033[3J"

#define L_BLACK "\033[30m"
#define L_RED "\033[1;31m"
#define L_GREEN "\033[1;32m"
#define L_YELLOW "\033[1;33m"
#define L_BLUE "\033[1;34m"
#define L_MAGENTA "\033[1;35m"
#define L_CYAN "\033[1;36m"
#define L_WHITE "\033[1;37m"
#define L_RESET "\033[0m"

//  `configM_LOG_LVL` is lower (less severe) than passed `level`
#define LOGGING_AT_OR_BELOW(level) \
	(configM_LOG_LVL <= level)

#define LOG(name, fmt, ...) printf(name str, ##__VA_ARGS__);

#if LOGGING_AT_OR_BELOW(LOG_LVL_ALL)
#define LOG_ALL(fmt, ...) printf(L_WHITE "ALL" L_RESET ": " fmt, ##__VA_ARGS__);
#else
#define LOG_ALL(fmt, ...) ;
#endif

#if LOGGING_AT_OR_BELOW(LOG_LVL_DEBUG)
#define LOG_DEBUG(fmt, ...) printf(L_GREEN "DEBUG" L_RESET ": " fmt, ##__VA_ARGS__);
#else
#define LOG_DEBUG(fmt, ...) ;
#endif

#if LOGGING_AT_OR_BELOW(LOG_LVL_INFO)
#define LOG_INFO(fmt, ...) printf(L_BLUE "INFO" L_RESET ": " fmt, ##__VA_ARGS__);
#else
#define LOG_INFO(fmt, ...) ;
#endif

#if LOGGING_AT_OR_BELOW(LOG_LVL_NOTICE)
#define LOG_NOTICE(fmt, ...) printf(L_CYAN "NOTICE" L_RESET ": " fmt, ##__VA_ARGS__);
#else
#define LOG_NOTICE(fmt, ...) ;
#endif

#if LOGGING_AT_OR_BELOW(LOG_LVL_WARNING)
#define LOG_WARNING(fmt, ...) printf(L_YELLOW "WARNING\a" L_RESET ": " fmt, ##__VA_ARGS__);
#else
#define LOG_WARNING(fmt, ...) ;
#endif

#if LOGGING_AT_OR_BELOW(LOG_LVL_ALERT)
#define LOG_ALERT(fmt, ...) printf(L_RED "ALERT\a" L_RESET ": " fmt, ##__VA_ARGS__);
#else
#define LOG_ALERT(fmt, ...) ;
#endif

#if LOGGING_AT_OR_BELOW(LOG_LVL_ERROR)
#define LOG_ERROR(fmt, ...) printf(L_BACKGROUND L_RED "ERROR\a" L_RESET ": " fmt, ##__VA_ARGS__);
#else
#define LOG_ERROR(fmt, ...) ;
#endif

#if LOGGING_AT_OR_BELOW(LOG_LVL_CRITICAL)
#define LOG_CRITICAL(fmt, ...) printf(L_BACKGROUND L_RED "CRITICAL\a" L_RESET ": " fmt, ##__VA_ARGS__);
#else
#define LOG_CRITICAL(fmt, ...) ;
#endif

#if LOGGING_AT_OR_BELOW(LOG_LVL_EMERGENCY)
#define LOG_EMERGENCY(fmt, ...) printf(L_BACKGROUND L_RED "EMERGENCY\a" L_RESET ": " fmt, ##__VA_ARGS__);
#else
#define LOG_EMERGENCY(fmt, ...) ;
#endif

#if LOGGING_AT_OR_BELOW(LOG_LVL_ALWAYS)
#define LOG_ALWAYS(fmt, ...) printf("" fmt, ##__VA_ARGS__);
#else
#define LOG_ALWAYS(fmt, ...) ;
#endif
