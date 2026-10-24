# Assignment 1: Library Catalogue

## Problem statement

A library wants a simple Java program to keep track of its books.

Each book has an **ISBN**, a **title**, an **author** and a **year of publication**. The library may own several **copies** of a book.

The program should let the librarian:

1. Add a book along with the number of copies.
2. Find a book by its ISBN.
3. Search for books by author, listed in order of title.
4. Lend a copy (one fewer on the shelf), and take it back on return (one more on the shelf).

**Rules**

- The same book (same ISBN) cannot be added twice.
- A book with an invalid ISBN (not exactly 13 digits), a blank title or author, or a year in the future is rejected.
- A copy cannot be lent when none are on the shelf.
- Errors are reported with exceptions.

There is no user interface and no database. Data is kept in memory.

## Implementation guidelines

Everyone must use the same constructs, as described below.

### 1. `Book`
- Four fields, `isbn`, `title`, `author` and `year`, all **`private final`**.
- Getters only. **No setters**, so a book cannot change after it is created.
- The constructor checks its inputs and throws **`IllegalArgumentException`** when:
  - the ISBN is null or not exactly 13 digits;
  - the title or author is null or blank;
  - the year is after the current year (use `Year.now().getValue()`).
- Override **`equals` and `hashCode` using only the ISBN**. Two `Book` objects with the same ISBN are the same book.
- Override **`toString`** to show the ISBN and title.

### 2. `Catalogue`
- Use **three `HashMap`s**, all keyed by ISBN:
  - `Map<String, Book> books`
  - `Map<String, Integer> totalCopies`
  - `Map<String, Integer> availableCopies`

| Method | What it does | Throws |
|---|---|---|
| `void addBook(Book book, int copies)` | Adds the book; all copies start on the shelf | `IllegalArgumentException` if copies < 1; `DuplicateBookException` if the ISBN exists |
| `Book findByIsbn(String isbn)` | Returns the book | `BookNotFoundException` |
| `List<Book> searchByAuthor(String name)` | Books whose author contains `name`, ignoring case, sorted by title. Use a **stream**: `filter`, then `sorted(Comparator.comparing(Book::getTitle))`, then `toList()`. Returns an empty list if nothing matches. | - |
| `void lendCopy(String isbn)` | One fewer copy on the shelf | `BookNotFoundException`; `NoCopiesAvailableException` if none are left |
| `void returnCopy(String isbn)` | One more copy on the shelf | `BookNotFoundException`; `IllegalStateException` if all copies are already on the shelf |
| `int availableCopies(String isbn)` | Number of copies on the shelf | `BookNotFoundException` |

### 3. Exceptions
- `BookNotFoundException`, `DuplicateBookException` and `NoCopiesAvailableException` are **provided**. Do not change them.
- **Never return `null`** to mean "not found". Throw the exception instead.

### 4. General
- Plain Java 17, with no external libraries.
- **Do not rename** classes or methods, or change their parameters. The grading tests depend on them.

## Getting started

You need JDK 17 or newer and Maven.

```
mvn test
```

At first every test fails with `UnsupportedOperationException: TODO`. Replace each `TODO` with your code until the tests pass. Work in this order:

1. `Book`: fields, constructor, getters
2. `Book`: `equals`, `hashCode`, `toString`
3. `Catalogue`: maps, `addBook`, `findByIsbn`, `availableCopies`
4. `Catalogue`: `searchByAuthor`
5. `Catalogue`: `lendCopy`, `returnCopy`

## Testing and grading

- **Write your own tests.** The tests in `src/test/java` are samples. Do not change them. Add your own test files next to them (for example `MyBookTest.java`, `MyCatalogueTest.java`) and test every rule above, especially the error cases.
- Your own tests are not graded. They are how you check your code before you submit.
- **Grading is automatic.** TAs run a larger set of tests against your `Book` and `Catalogue`.
- **Code that does not compile scores zero.** Run `mvn test` before you submit.

## Submission

- **Deadline:** 24 October 2026
- **Where:** upload on the LMS
- **What:** zip the **whole project folder** and name it with your roll number, for example `MT2026001.zip`.
  - `pom.xml` must be at the top level of the zip, next to `src/`.
  - Leave out the `target/` folder.
- Before you zip, run `mvn test` and check that the project compiles.
