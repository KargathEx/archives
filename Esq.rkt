;;from https://www2.ccs.neu.edu/racket/pubs/icfp99-ffkf.pdf in version 7.6 update 
#lang racket/gui

;; The repl editor class
(define esq-text%
  (class text%
    ;; lexical access to inherited methods:
    (inherit insert last-position get-text erase)
    ;; private fields:
    (define prompt-pos 0)
    (define locked? #t)
    ;; override can-insert? to block pre-prompt inserts:
    (define/augment can-insert?
      (lambda (start len)
        (and (>= start prompt-pos) (not locked?))))
    ;; override can-delete? to block pre-prompt deletes:
    (define/augment can-delete?
      (lambda (start end)
        (and (>= start prompt-pos) (not locked?))))
    ;; override on-char to detect Enter/Return:
    (define/override on-char
      (lambda (c)
        (super on-char c)
        (when (and (eq? (send c get-key-code)
                        #\return) (not locked?))
          (set! locked? #t)
          (evaluate
           (get-text prompt-pos (last-position))))))
    ;; method to insert a new prompt
    (define/public new-prompt
      (lambda ()
        (queue-output (lambda ()
                        (set! locked? #f)
                        (insert "> ")
                        (set! prompt-pos (last-position))))))
    ;; method to display output
    (define/public output
      (lambda (str) (queue-output (lambda ()
                                    (let ((was-locked? locked?))
                                      (set! locked? #f)
                                      (insert str)
                                      (set! locked? was-locked?))))))
    ;; method to reset the repl:
    (define/public reset
      (lambda ()
        (set! locked? #f) (set! prompt-pos 0) (erase) (new-prompt)))
    ;; initialize superclass-defined state:
    (super-new)
    ;; create the initial prompt:
    (new-prompt)))

(define esq-eventspace (current-eventspace))
(define (queue-output proc)
  (parameterize ((current-eventspace esq-eventspace))
    (queue-callback proc #f))) ;; GUI creation
(define frame
  (make-object frame% "SchemeEsq" #f 400 200))
(define reset-button
  (make-object button% "Reset" frame
               (lambda (b e) (reset-program))))
(define repl-editor (make-object esq-text%))
(define repl-display-canvas
  (make-object editor-canvas% frame))
(send repl-display-canvas set-editor repl-editor) (send frame show #t)
;; User space initialization
(define user-custodian (make-custodian))
(define user-output-port (make-output-port
                          'eqs
                          always-evt
                          ;; string printer:
                          (lambda (bstr start end buffer? enable-break?)
                            (send repl-editor output (bytes->string/utf-8 bstr #\? start end))
                            (- end start))
                          ;; closer:
                          (lambda () 'nothing-to-close)))
(define user-eventspace
  (parameterize ([current-custodian user-custodian]
                 [current-namespace (make-gui-namespace)])
    (make-eventspace)))
;; Evaluation and resetting
(define (evaluate expr-str)
  (parameterize ((current-eventspace user-eventspace))
    (queue-callback (lambda ()
                      (current-output-port user-output-port)
                      (with-handlers ((exn?
                                       (lambda (exn) (display
                                                      (exn-message exn)))))
                        (println (eval (read (open-input-string expr-str)))))
                      (send repl-editor new-prompt)))))
(define (reset-program)
  (custodian-shutdown-all user-custodian)
  (set! user-custodian (make-custodian))
  (parameterize ([current-custodian user-custodian]
                 [current-namespace (make-gui-namespace)])
    (set! user-eventspace (make-eventspace)))
  (send repl-editor reset))
