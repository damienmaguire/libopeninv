/*              category     name         unit       min     max     default id */
#define PARAM_LIST \
    VALUE_ENTRY(amp,            "dig",   2013 ) \
    VALUE_ENTRY(pot,            "dig",   2015 ) \
    PARAM_ENTRY("inverter",   ocurlim,     "A",       -65536, 65536,  100,    22  )

extern const char* errorListString;
