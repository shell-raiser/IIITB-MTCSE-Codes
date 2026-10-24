package edu.esd.library;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * The library catalogue: the books the library owns and how many copies of each.
 *
 * <p>Guidelines:
 * <ul>
 *   <li>Store books in a {@code Map<String, Book>} keyed by ISBN.</li>
 *   <li>Store copy counts in two maps keyed by ISBN:
 *       {@code Map<String, Integer> totalCopies} and {@code Map<String, Integer> availableCopies}.</li>
 *   <li>Use {@code HashMap} for all three.</li>
 *   <li>Never return null to mean "not found" -- throw the exception instead.</li>
 * </ul>
 */
public class Catalogue {

    // TODO: declare the three maps described above
    private final Map<String, Book> books= new HashMap<>();
    private final Map<String, Integer> totalCopies= new HashMap<>();
    private final Map<String, Integer> availableCopies= new HashMap<>();
    
    /**
     * Adds a new book with the given number of copies. All copies start on the shelf.
     *
     * @throws IllegalArgumentException if copies is less than 1
     * @throws DuplicateBookException   if a book with the same ISBN already exists
     */
    public void addBook(Book book, int copies) {
        // throw new UnsupportedOperationException("TODO: implement addBook");
        if (copies < 1) {
            throw new IllegalArgumentException("Number of copies must be at least 1");
        }
        if (books.containsKey(book.getIsbn())) {
            throw new DuplicateBookException(book.getIsbn());
        }
        books.put(book.getIsbn(), book);
        totalCopies.put(book.getIsbn(), copies);
        availableCopies.put(book.getIsbn(), copies);
    }

    /**
     * @throws BookNotFoundException if there is no book with this ISBN
     */
    public Book findByIsbn(String isbn) {
        // throw new UnsupportedOperationException("TODO: implement findByIsbn");
        if (!books.containsKey(isbn)) {
            throw new BookNotFoundException(isbn);
        }
        return books.get(isbn);
    }

    /**
     * Finds books whose author contains {@code name}, ignoring upper/lower case.
     * For example "bloch" matches "Joshua Bloch".
     *
     * <p>Use a stream: filter, then sorted(Comparator.comparing(Book::getTitle)), then toList().
     *
     * @return matching books sorted by title; an empty list if none match
     */
    public List<Book> searchByAuthor(String name) {
        // throw new UnsupportedOperationException("TODO: implement searchByAuthor");
        return books.values().stream()
                .filter(book -> book.getAuthor().toLowerCase().contains(name.toLowerCase()))
                .sorted((b1, b2) -> b1.getTitle().compareTo(b2.getTitle()))
                .toList();
    }

    /**
     * Lends one copy: one fewer copy on the shelf.
     *
     * @throws BookNotFoundException       if there is no book with this ISBN
     * @throws NoCopiesAvailableException  if no copy is on the shelf
     */
    public void lendCopy(String isbn) {
        // throw new UnsupportedOperationException("TODO: implement lendCopy");
        if (!books.containsKey(isbn)) {
            throw new BookNotFoundException(isbn);
        }
        int available = availableCopies.get(isbn);
        if (available <= 0) {
            throw new NoCopiesAvailableException(isbn);
        }
        availableCopies.put(isbn, available - 1);
        return;
    }

    /**
     * Takes a copy back: one more copy on the shelf.
     *
     * @throws BookNotFoundException  if there is no book with this ISBN
     * @throws IllegalStateException  if all copies are already on the shelf
     */
    public void returnCopy(String isbn) {
        // throw new UnsupportedOperationException("TODO: implement returnCopy");
        if (!books.containsKey(isbn)) {
            throw new BookNotFoundException(isbn);
        }
        int available = availableCopies.get(isbn);
        int total = totalCopies.get(isbn);
        if (available >= total) {
            throw new IllegalStateException("All copies are already on the shelf");
        }
        availableCopies.put(isbn, available + 1);
    }

    /**
     * @return the number of copies on the shelf right now
     * @throws BookNotFoundException if there is no book with this ISBN
     */
    public int availableCopies(String isbn) {
        // throw new UnsupportedOperationException("TODO: implement availableCopies");
        if (!books.containsKey(isbn)) {
            throw new BookNotFoundException(isbn);
        }
        return availableCopies.get(isbn);
    }
}
