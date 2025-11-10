The purpose of this component is to allow for length of pull (google it) functionality for your stocks along with the ability to fold/unfold. You can collapse and extend a stock like you can on an M4 as well as fold/unfold your stock like on an under folding AK

![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/a51f3a6d-939d-4142-97c9-b0a0065f0335)

# Details Panel
## Initialize
### Can Be Folded
This option allows you to be able to fold/unfold the stock when true, if false this functionality is ignored.
### Can Be Affected By Offset
This option ties hand in hand with the attachment system out of the box. The SKGFirearmComponent will bind to the attachment component of the stock automatically. When you extend or collapse the stock, it will automatically set the offset in the stock component.
### Unfolded Length Of Pull
This is the length of pull for when the stock is unfolded.
### Folded Length Of Pull
This is the length of pull for when the stock is Folded. For example, on an under folding AK the Unfolded Length Of Pull might be 0 but the Folded Length Of Pull might be -3.


# Functions
## SKGStock category
### SetOffset
This function is what the attachment system/SKGFirearmComponent automatically handles. For example, if you extend a M4 stock on the buffer tube, you would pass in a value greater than 0 and this would alter your length of pull in game. If [Can Be Affected By Offset](#can-be-affected-by-offset) is false, it nothing will happen.
### Fold
This function will trigger the stock to fold if [Can Be Folded](#can-be-folded) is true.
### Unfold
This function will trigger the stock to unfold if [Can Be Folded](#can-be-folded) is true.
### ToggleFold
If the stock is folded, Unfold will be called. If the stock is unfolded, Fold will be called.
### IsFolded
Returns true or false depending on if the stock is folded.
### GetLengthOfPull
This returns the current value of the length of pull including the offset (if length of pull is 2, and offset is 3, -1 is returned (offset is subtracted from length of pull).

## SKGStock Initialize Category
### CanBeFolded
This returns whether or not the stock can be folded.
### GetUnfoldedLengthOfPull
This returns the default value set for the [unfolded length of pull](#unfolded-length-of-pull).
### GetFoldedLengthOfPull
This returns the default value set for the [folded length of pull](#folded-length-of-pull).

# Events
### OnFolded
This event fires each time the stock is folded or unfolded. Passes in the folded state.
### OnLengthOfPullChanged
This event fires each time the length of pull on the stock changes. Passes in the new length of pull.