package edu.esd.library;

/** Provided. Do not change. */
public class NoCopiesAvailableException extends RuntimeException {

    public NoCopiesAvailableException(String isbn) {
        super("No copies available for ISBN " + isbn);
    }
}
