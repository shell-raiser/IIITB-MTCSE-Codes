package edu.esd.library;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

/** Sample tests. Add your own tests in this folder. */
class BookTest {

    @Test
    void createsAValidBook() {
        Book book = new Book("9780134685991", "Effective Java", "Joshua Bloch", 2018);

        assertEquals("9780134685991", book.getIsbn());
        assertEquals("Effective Java", book.getTitle());
        assertEquals("Joshua Bloch", book.getAuthor());
        assertEquals(2018, book.getYear());
    }

    @Test
    void rejectsIsbnWithTwelveDigits() {
        assertThrows(IllegalArgumentException.class,
                () -> new Book("978013468599", "Effective Java", "Joshua Bloch", 2018));
    }

    @Test
    void rejectsBlankTitle() {
        assertThrows(IllegalArgumentException.class,
                () -> new Book("9780134685991", " ", "Joshua Bloch", 2018));
    }

    @Test
    void booksWithTheSameIsbnAreEqual() {
        Book a = new Book("9780134685991", "Effective Java", "Joshua Bloch", 2018);
        Book b = new Book("9780134685991", "Effective Java, 3rd edition", "J. Bloch", 2017);

        assertEquals(a, b);
        assertEquals(a.hashCode(), b.hashCode());
    }

    @Test
    void booksWithDifferentIsbnsAreNotEqual() {
        Book a = new Book("9780134685991", "Effective Java", "Joshua Bloch", 2018);
        Book b = new Book("9780132350884", "Effective Java", "Joshua Bloch", 2018);

        assertNotEquals(a, b);
    }
}
