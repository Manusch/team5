(format #t "<CFG start~%")

(use-modules (ice-9 optargs))

(load (string-append (getcwd) "/map.shader"))

(define cam (make-perspective-camera "cam" (list 0 -1 0.3) (list 0.05 1 0.5) (list 0.01	0.5	-1) 45 1 0.01 10))
(use-camera cam)

(format #t "CFG end>~%")
