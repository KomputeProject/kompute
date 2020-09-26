extends Node2D

# Called when the node enters the scene tree for the first time.
func _ready():

    var xi = [0, 1, 1, 1, 1, 1]
    var xj = [0, 0, 0, 1, 1, 1]

    var y_train_1 = [0, 0, 0, 1, 1, 1]

    print("Training with " + str(y_train_1))
    $KomputeNode.train(y_train_1, xi, xj)

    print("Now running prediction with " + str(xi) + " and " + str(xj))
    print($KomputeNode.predict(xi, xj))

    # We can also reference the class as named in editor
    # and create a new instance
    var s = KomputeSummator.new()

    # We can use a new prediciton value to see how weights change
    var y_train_2 = [0, 0, 1, 1, 1, 1]

    print("\nTraining with " + str(y_train_2))
    s.train(y_train_2, xi, xj)

    print("Now running prediction with " + str(xi) + " and " + str(xj))
    print(s.predict(xi, xj))

