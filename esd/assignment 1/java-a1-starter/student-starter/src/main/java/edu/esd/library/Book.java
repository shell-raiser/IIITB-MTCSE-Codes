package edu.esd.library;

import java.time.Year;

/**
 * A book in the library catalogue.
 *
 * <p>Guidelines:
 * <ul>
 *   <li>Fields: isbn, title, author, year -- all {@code private final}. No setters.</li>
 *   <li>Two books are equal if and only if they have the same ISBN.</li>
 * </ul>
 */
public class Book {

    // TODO: declare the four fields (private final)
    private final String isbn;
    private final String title;
    private final String author;
    private final int year;
    /**
     * Creates a book.
     *
     * @throws IllegalArgumentException if
     *     <ul>
     *       <li>isbn is null or is not exactly 13 digits (e.g. "9780134685991"),</li>
     *       <li>title or author is null or blank,</li>
     *       <li>year is after the current year.</li>
     *     </ul>
     */
    public Book(String isbn, String title, String author, int year) {
        this.isbn = isbn;
        this.title = title;
        this.author = author;
        this.year = year;
        if (isbn == null || !isbn.matches("\\d{13}")) {
            throw new IllegalArgumentException("ISBN must be exactly 13 digits");
        }
        if (title == null || title.isBlank()) {
            throw new IllegalArgumentException("Title cannot be null or blank");
        }
        if (author == null || author.isBlank()) {
            throw new IllegalArgumentException("Author cannot be null or blank");
        }
        if (year > Year.now().getValue()) {
            throw new IllegalArgumentException("Year cannot be in the future");
        }
    }

    public String getIsbn() {
        return isbn;
    }

    public String getTitle() {
        return title;
    }

    public String getAuthor() {
        return author;
    }

    public int getYear() {
        return year;
    }

    // TODO: override equals and hashCode (use ONLY the ISBN), and toString (show ISBN and title).
    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Book book = (Book) o;
        return java.util.Objects.equals(isbn, book.isbn);
    }

    @Override
    public int hashCode() {
        return java.util.Objects.hash(isbn);
    }

    @Override
    public String toString() {
        return "Book{" +
                "isbn='" + isbn + '\'' +
                ", title='" + title + '\'' +
                '}';
    }
}
