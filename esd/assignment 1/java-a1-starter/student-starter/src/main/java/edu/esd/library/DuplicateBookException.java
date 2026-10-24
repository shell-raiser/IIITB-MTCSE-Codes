package edu.esd.library;

/** Provided. Do not change. */
public class DuplicateBookException extends RuntimeException {

    public DuplicateBookException(String isbn) {
        super("A book with this ISBN already exists: " + isbn);
    }
}
