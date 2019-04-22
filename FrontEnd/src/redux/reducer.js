import * as actions from "./actions"
import {combineReducers} from "redux"

function RemarkDetails(state = {}, action) {
    switch (action.type) {
        case actions.SET_CARD_ID : {
            return {...state, cardId: action.cardId}
        }
        default: {
            return state;
        }
    }
}

const reducer = combineReducers({
    RemarkDetails
})

export default reducer