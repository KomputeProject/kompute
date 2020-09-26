extends Node2D

# Called when the node enters the scene tree for the first time.
func _ready():

    print("hello")

    # Use existing node
    print($KomputeNode.get_total())

    $KomputeNode.add([10.0, 20.0])
    print($KomputeNode.get_total())

    $KomputeNode.add([10.0, 20.0])
    print($KomputeNode.get_total())

    # Create new instance
    var s = KomputeSummator.new()

    # This will print 0 as it's a new instance
    print(s.get_total())

    # Now we can again send further commands
    s.add([10.0, 20.0])
    print(s.get_total())

    s.add([10.0, 20.0])
    print(s.get_total())

