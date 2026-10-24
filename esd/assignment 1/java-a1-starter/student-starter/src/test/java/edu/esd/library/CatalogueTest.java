package edu.esd.library;

import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

import java.util.List;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

/** Sample tests. Add your own tests in this folder. */
class CatalogueTest {

    private final Book effectiveJava = new Book("9780134685991", "Effective Java", "Joshua Bloch", 2018);
    private final Book javaPuzzlers = new Book("9780321336781", "Java Puzzlers", "Joshua Bloch", 2005);
    private final Book cleanCode = new Book("9780132350884", "Clean Code", "Robert C. Martin", 2008);

    private Catalogue catalogue;

    @BeforeEach
    void setUp() {
        catalogue = new Catalogue();
    }

    @Test
    void addedBookCanBeFound() {
        catalogue.addBook(effectiveJava, 2);

        assertEquals(effectiveJava, catalogue.findByIsbn("9780134685991"));
        assertEquals(2, catalogue.availableCopies("9780134685991"));
    }

    @Test
    void addingTheSameBookTwiceFails() {
        catalogue.addBook(effectiveJava, 1);

        assertThrows(DuplicateBookException.class, () -> catalogue.addBook(effectiveJava, 1));
    }

    @Test
    void searchByAuthorIgnoresCaseAndSortsByTitle() {
        catalogue.addBook(javaPuzzlers, 1);
        catalogue.addBook(cleanCode, 1);
        catalogue.addBook(effectiveJava, 1);

        assertEquals(List.of(effectiveJava, javaPuzzlers), catalogue.searchByAuthor("bloch"));
    }

    @Test
    void lendingTheLastCopyLeavesNoneOnTheShelf() {
        catalogue.addBook(cleanCode, 1);

        catalogue.lendCopy("9780132350884");

        assertEquals(0, catalogue.availableCopies("9780132350884"));
        assertThrows(NoCopiesAvailableException.class, () -> catalogue.lendCopy("9780132350884"));
    }

    @Test
    void returnedCopyGoesBackOnTheShelf() {
        catalogue.addBook(cleanCode, 1);
        catalogue.lendCopy("9780132350884");

        catalogue.returnCopy("9780132350884");

        assertEquals(1, catalogue.availableCopies("9780132350884"));
    }
}
