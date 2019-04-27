import Card from '../component/card/Card.js';
import { connect } from 'react-redux';
import {
  createNewComment,
  createNewLike,
  deleteLike,
} from '../store/actions'

const mapStateToProps = state => ({
  username: state.username,
  userId: state.userId,
  obiews: state.obiews,
})

const mapDispatchToProps = dispatch => ({
  onCreateNewComment: (obiewId, comment) => dispatch(createNewComment(obiewId, comment)),
  onCreateNewLike: (obiewId, like) => dispatch(createNewLike(obiewId, like)),
  onDeleteLike: (obiewId, userId) => dispatch(deleteLike(obiewId, userId)),
})

export default connect(mapStateToProps, mapDispatchToProps)(Card)
