#pragma once

/**
 * \brief The level on which a log-message is logged
 */
enum class LogLevel
{
    /**
     * \brief Log-level for debugging-related log-messages. This is the lowest level (highest verbosity)
     */
    Debug,
    /**
     * \brief Log-level for most log-messages. This is the second-lowest level (second-highest verbosity)
     */
    Verbose,
    /**
     * \brief Log-level for important log-messages. This is the second-highest level (second-lowest verbosity)
     */
    Important,
    /**
     * \brief Log-level for error messages. This is the highest level (lowest verbosity)
     */
    Error
};